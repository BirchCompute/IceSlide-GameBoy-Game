INCLUDE "includes/hardware.inc"
INCLUDE "includes/hardware_compat.inc"

SECTION "Current Level", WRAM0
	wCurrentLevel:: ds 2

SECTION "Level data table", romx
	def offsetTiles equ 0
	def offsetMap equ 4
	def offsetObj equ 8
	def offsetStart equ 12
	def offsetNextLevel equ 14
	LevelTable::
		LevelPlinko::
			dw PlinkoTiles, PlinkoTilesLen
			dw PlinkoMap, PlinkoMapLen
			dw BallTile, BallTileLen
			db $90 ; Entrance_Y
			db $98 ; Entrance_X
			dw LevelTest ; Next level
		LevelTest::
			dw TestTiles, TestTilesLen
			dw TestMap, TestMapLen
			dw BallTile, BallTileLen
			db $20 ; Entrance_Y
			db $18 ; Entrance_X
			dw LevelPlinko ; Next level
		LevelScroll::
			dw ScrollTiles, ScrollTilesLen
			dw ScrollMap, ScrollMapLen
			dw BallTile, BallTileLen
			db $20 ; Entrance_Y
			db $18 ; Entrance_X
			dw LevelPlinko ; Next level

SECTION "Level data", romx
	; level graphics

	; Plinko
		PlinkoTiles:: INCBIN "Art/Plinko/PlinkoTiles.2bpp"
			PlinkoTilesEnd::
		PlinkoMap:: INCBIN "Art/Plinko/PlinkoMap.tilemap"
			PlinkoMapEnd::
		def PlinkoTilesLen equ PlinkoTilesEnd-PlinkoTiles
		def PlinkoMapLen equ PlinkoMapEnd-PlinkoMap

	; Test
		TestTiles:: INCBIN "Art/Test/TestTiles.2bpp"
			TestTilesEnd::
		TestMap:: INCBIN "Art/Test/TestMap.tilemap"
			TestMapEnd::
		def TestTilesLen equ TestTilesEnd-TestTiles
		def TestMapLen equ TestMapEnd-TestMap

	; Scroll
		ScrollTiles:: INCBIN "Art/Scroll/ScrollTiles.2bpp"
			ScrollTilesEnd::
		ScrollMap:: INCBIN "Art/Scroll/ScrollMap.tilemap"
			ScrollMapEnd::
		def ScrollTilesLen equ ScrollTilesEnd-ScrollTiles
		def ScrollMapLen equ ScrollMapEnd-ScrollMap

	; object graphics
		BallTile:: INCBIN "Art/BallTile.2bpp"
		BallTileEnd::
		def BallTileLen equ BallTileEnd-BallTile

SECTION "Level Functions ", romx
	GetLevel::
		; HL will point to the current level entry
		ld hl, wCurrentLevel
		ld a, [hl+]
		ld h, [hl]
		ld l, a  ; HL = pointer to level entry
		ret
	NextLevel::
		; get level
			call GetLevel
		; get next level table location
			ld bc, offsetNextLevel
			add hl, bc
		; get location recorded on table
			ld a, [hl+]
			ld h, [hl]
			ld l, a
		;move next level address to wCurrentLevel
			ld a, l
			ld [wCurrentLevel], a
			ld a, h
			ld [wCurrentLevel+1], a 
		ret
	ChangeLevel::
		;Clear the tile map
			ld bc, TILE_SIZE * $7F
			ld hl, TILEMAP0
			call Clear
		; Clear the tile set
			ld bc, TILE_SIZE * $7F
			ld hl, STARTOF(VRAM) + $100 * TILE_SIZE ; make a constant
			call Clear

		; Copy tile map
			call GetLevel
			ld bc, offsetMap
			add hl, bc
			ld a, [hl+]
			ld e, a
			ld a, [hl+]
			ld d, a
			ld a, [hl+]
			ld c, a
			ld a, [hl+]
			ld b, a
			ld hl, TILEMAP0 
			call Copy
		; copy tile set
			call GetLevel
			ld a, [hl+]
			ld e, a
			ld a, [hl+]
			ld d, a
			ld a, [hl+]
			ld c, a
			ld a, [hl+]
			ld b, a
			ld hl, STARTOF(VRAM) + $100 * TILE_SIZE ; make a constant
			call Copy

		;clear OAM
			call Clear_OAM
		; Copy object tile
			call GetLevel
			ld bc, offsetObj
			add hl, bc
			ld a, [hl+]
			ld e, a
			ld a, [hl+]
			ld d, a
			ld a, [hl+]
			ld c, a
			ld a, [hl+]
			ld b, a
			ld hl, $8000
			call Copy
		; Object location
			call GetLevel
			ld bc, offsetStart
			add hl, bc
			ld a, [hli]
			ld b, [hl]
			ld hl, _OAMRAM
			ld [hli], a
			ld [hl], b

		ret
