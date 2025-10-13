INCLUDE "includes/hardware.inc"
INCLUDE "includes/hardware_compat.inc"

SECTION "Func", romx

	Copy:
  		; Subroutine to copy bc bytes from de to hl
  		ld a, [de]
  		ld [hli], a
  		inc de
  		dec bc
  		ld a, b
  		or a, c
  		jr nz, Copy
  		ret

	Clear:
		; Subroutine to reset bc bytes after hl
		xor a
		ld [hli], a
		dec bc
		ld a, b
		or a, c
		jr nz, Clear
		ret

	Wait_vblank:
		;Wait for Y position below 
			ldh a, [rLY]
			cp LY_VBLANK
			jr c, Wait_vblank
			ret 
  	wait_h_vblanks:
		call Wait_vblank
		dec h
		jp nz, wait_h_vblanks
		ret

	Clear_OAM::
		ld a, 0
		ld b, 160
		ld hl, $FE00
    	.loop
			ld [hli], a
			dec b
			jp nz, .loop

	LCD_off::
	; Turn the LCD off
		ld a, LCDC_OFF
		ldh [rLCDC], a
		ret

	LCD_on::
	; Turn the LCD on
	ld a, LCDCF_ON  | LCDCF_BGON|LCDCF_OBJON
	ldh [rLCDC], a
	ret

	UpdateKeys::
	; Poll half the controller
		ld a, P1F_GET_BTN
		call .onenibble
		ld b, a ; B7-4 = 1; B3-0 = unpressed buttons

	; Poll the other half
		ld a, P1F_GET_DPAD
		call .onenibble
		swap a ; A7-4 = unpressed directions; A3-0 = 1
		xor a, b ; A = pressed buttons + directions
		ld b, a ; B = pressed buttons + directions

	; And release the controller
		ld a, P1F_GET_NONE
		ldh [rP1], a

	; Combine with previous wCurKeys to make wNewKeys
		ld a, [wCurKeys]
		xor a, b ; A = keys that changed state
		and a, b ; A = keys that changed to pressed
		ld [wNewKeys], a
		ld a, b
		ld [wCurKeys], a
		ret

	.onenibble
	   ldh [rP1], a ; switch the key matrix
	   call .knownret ; burn 10 cycles calling a known ret
	   ldh a, [rP1] ; ignore value while waiting for the key matrix to settle
	   ldh a, [rP1]
	   ldh a, [rP1] ; this read counts
	   or a, $F0 ; A7-4 = 1; A3-0 = unpressed keys
	.knownret
	   ret

	multiply:
	; @param b: factor
	; @param c: factor
	; @return a: b*c 
		xor a
		or b
		jp nz, .notzero1
			ret
		.notzero1
		or c
		jp nz, .notzero2
			ret
		.notzero2
		.loop
			add b
			dec c
			jp nz, .loop
			ret

	divide:
	; @param b: dividend
	; @param c: divisor
	; Overwrites d
	; @return a: b/c, but b/0 returns 0
	xor a
	or b
	jp nz, .notzero1
    	ret
    .notzero1
	or c
	jp nz, .notzero2
    	ret
	.notzero2
	ld a, b
	ld d, 0
	.loop
    	sub c
    	inc d
    	jp nc, .loop
    	ld a, d
    	ret 

	Addr_at_hl_to_hl::
		ld a, [hl+]
		ld h, [hl]
		ld l, a 
		ret 

	bc_decrease_by_de::
		ld a, c
		sub e
		ld c, a
		ld a, b
		sbc d
		ld b, a

	; Convert object position to tilemap address
	; hl = $9800 + [_OAMRAM] + [_OAMRAM+1] * 32
	; @return hl: tile address
	; Runs over hl and bc
	GetTileByPixel:
    	; Divide by 8
    	; Then multiply the Y position by 32.
    		ld a, [_OAMRAM]
    		and a, %11111000
    		ld l, a
    		ld h, 0
    	; Now we have the position * 8 in hl
    		add hl, hl ; position * 16
    		add hl, hl ; position * 32
    	; Convert the X position to an offset.
    		ld a, [_OAMRAM+1]
    		srl a ; a / 2
    		srl a ; a / 4
    		srl a ; a / 8
    	; Add the two offsets together.
    		add a, l
    		ld l, a
    		adc a, h
    		sub a, l
    		ld h, a
    	; Add the offset to the tilemap's base address, and we are done!
    		ld bc, $97BF
    		add hl, bc
    		ret

	ClearStatus::
		xor a
		ld [wStatus], a
		ret
  