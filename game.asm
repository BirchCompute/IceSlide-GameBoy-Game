
;includes
	INCLUDE "includes/hardware.inc"
	INCLUDE "includes/hardware_compat.inc"
	INCLUDE "includes/functions.inc"
	INCLUDE "includes/levels.inc"

SECTION "Global Variables", WRAM0[$c006]
	wStatus:: db
	; A, B, Select, Start, Right, Left, Up, Down
	wCurKeys:: db
	wNewKeys:: db

SECTION "Global Constants", romx
	def TickRate equ 10

SECTION "Header", ROM0[$100]
	jp EntryPoint
	ds $150 - @, 0 ; Make room for the header
	EntryPoint:
		; Shut down audio circuitry
			xor a
			ldh [rAUDENA], a
		;handle graphics
			; Do not change VRAM or turn LCD off outside of VBlank
				call Wait_vblank
				call LCD_off
			; Load the first level
				ld hl, LevelTest
				ld a, l
				ld [wCurrentLevel], a
				ld a, h
				ld [wCurrentLevel+1], a
				call ChangeLevel

		; Initialize global variables
			xor a
			ld [wCurKeys], a
			ld [wNewKeys], a
			ld [wStatus], a

			; VRAM changes are done, so we can turn the LCD back on
				call LCD_on
			; During the first (blank) frame, initialize display registers
				ld a, %11100100
				ld [rBGP], a
				ld a, %11100100
				ld [rOBP0], a

	Main:
	; This divides CPU clock to game tick rate
		ld h, TickRate
		call wait_h_vblanks

	; Check for status
		ld a, [wStatus]
		and a, %00001111
	; Either wait for input to change wStatus,
		jp z, Status_Update
		; or move in the direction of wStatus
			jp Collide_or_Move



SECTION "Game functions", romx
	Status_Update::
		call UpdateKeys
		.right
			ld a, [wCurKeys]
			bit 4, a
			jp z, .left
				ld a, %00001000
				ld [wStatus], a
				jp Main
		.left
			ld a, [wCurKeys]
			bit 5, a
			jp z, .up
				ld a, %00000100
				ld [wStatus], a
				jp Main
		.up
			ld a, [wCurKeys]
			bit 6, a
			jp z, .down
				ld a, %00000010
				ld [wStatus], a
				jp Main
		.down
			ld a, [wCurKeys]
			bit 7, a
			jp z, Main
				ld a, %00000001
				ld [wStatus], a
				jp Main

	Collide_or_Move::
		ld a, [wStatus]
		;horizontal
		.right
			;moving right?
			bit 3, a
			jp z, .left
				;are we on a tile?
				call On_Grid
				jp nz, .moveright
					call GetTileByPixel
					;check for goal
						ld a, [hl]
						or a
						jp z, .win
					;check for collision right
						inc hl
						ld a, [hl]
						and %11110000
						jp z, .moveright
							call ClearStatus
							jp Main
						.moveright
							ld hl, _OAMRAM+1
							inc [hl]
							jp Main
		.left
			;moving left?
			bit 2, a
			jp z, .up
				;are we on a tile?
				call On_Grid
				jp nz, .moveleft
					call GetTileByPixel
					;check for goal
						ld a, [hl]
						or a
						jp z, .win
					;check for collision left
						dec hl
						ld a, [hl]
						and %11110000
						jp z, .moveleft
							call ClearStatus
							jp Main
						.moveleft
							ld hl, _OAMRAM+1
							dec [hl]
							jp Main
		;vertical
		.up
			;moving up?
			bit 1, a
			jp z, .down
				;are we on a tile?
				call On_Grid
				jp nz, .moveup
					call GetTileByPixel
					;ld b,b
					;check for goal
						ld a, [hl]
						or a
						jp z, .win
					;check for collision up
						;subtract two rows from our tilemap location
						;code from pokecrystal wiki
							ld a, l
							sub $20
							ld l, a
							jr nc, .no_carry
							dec h
							.no_carry
						ld a, [hl]	
						and %11110000
						jp z, .moveup
							call ClearStatus
							jp Main
						.moveup
							ld hl, _OAMRAM
							dec [hl]
							jp Main
		.down
			;moving down?
			bit 0, a
			jp z, Main
				;are we on a tile?
				call On_Grid
				jp nz, .movedown
					call GetTileByPixel
					;check for goal
						ld a, [hl]
						or a
						jp z, .win
					;Check for collision down
							ld bc, $20
							add hl, bc
							ld a, [hl]
							and %11110000
							jp z, .movedown
								call ClearStatus
								jp Main
							.movedown
								ld hl, _OAMRAM
								inc [hl]
								jp Main
		.win
			;ld b,b
			call ClearStatus
			call LCD_off
			call NextLevel
			call ChangeLevel
			call LCD_on
			jp Main

	On_Grid::
		;If Z, then you're on the grid
		ld a, [_OAMRAM]
		and %00000111
		ret nz
		ld a, [_OAMRAM+1]
		and %00000111
		ret nz
		ret
