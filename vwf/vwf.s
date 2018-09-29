;MegaMan Xtreme VWF code v 0.1w
;_ocean (2005-2006)

.MEMORYMAP
SLOTSIZE $4000
DEFAULTSLOT 1
SLOT 0 $0000
SLOT 1 $4000
.ENDME

.ROMBANKSIZE $4000
.ROMBANKS 64

.COMPUTEGBCHECKSUM
.COMPUTEGBCOMPLEMENTCHECK

.BACKGROUND "work.gbc"



.DEFINE font        $4000
.DEFINE Bit_Pos     $C0
.DEFINE Tile_Pos    $C1
.DEFINE Ct_Line     $C2
.DEFINE Charx       $C3
.DEFINE Blankspace  $8BC0

SECTION "WIDTH", HOME[$86]
widthtbl : incbin "width.bin"

SECTION "BAO", HOME[$1B5A]
jp wooshi
fottuto:

SECTION "JMP", HOME[$2A42]
jp vwfroutine

wooshi:
ldh a,[$C4]
or a
jr z, miaobab
xor a
ldh [$C4],a
ldh [Bit_Pos],a
ldh [Tile_Pos],a
miaobab:
ld a,[hl+]
and a
ret z
jp fottuto

SECTION "VWF",HOME[$3E9E]
;//////////////////////////VWF ROUTINE////////////////////////
vwfroutine:
ld a,01
ldh [$C4], a
ld a,[$7FFE]
push af
ld a,[$D3B9]
ld [$2000],a
ld c,[hl]
inc hl
pop af
ld [$2000],a
ld a,c

;vwf bwahahahahah... XD

or a ;(spazio)
jr nz, nospazio

jp nofine
nospazio:

push af
push af
push bc
push de
push hl
ld a,$30
ldh [$F7],a
call $2A14
pop hl
pop de
pop bc
pop af
pop af

cp $FF ;(a capo)
jr nz, noacapo
ldh a,[Tile_Pos]
cp 16
jr c, .label1
.bao:
xor a
ldh [Tile_Pos],a
jr .label2
.label1:
ld a,[$D3B3]
or a
jr z, .bao
ld a,16
ldh [Tile_Pos],a
.label2:
xor a
ldh [Bit_Pos],a
ld hl,$D3B2
ld a,[hl]
xor a,01
ld [hl+],a
ld  [hl],00
cp a,01
ret
noacapo:

cp $FE ;(fine)
jr nz, nofine
xor a
ldh [Bit_Pos],a
ldh [Tile_Pos],a
dec a
ret
nofine:

ldh [Charx],a
ld h,0
ld l,a
ld bc, font
call calcolo_offset8 ;in hl c'è il font read pointer
push hl
ldh a,[Tile_Pos]
ld h,0
ld l,a
ld bc, Blankspace ;(spazio vuoto in vram)
call calcolo_offset16  ;in hl c'è il ram write pos. pointer
ld c,8
pop de ;in de c'è il font read pointer
push hl ;nello stack abbiamo il ram write pos. pointer
ld hl,$D014
ld a,d
ld [hl+],a
ld a,e
ld [hl],e
write_loop:
ld hl,$D014
ld a,[hl+]
ld l,[hl]
ld h,a
ldh a,[Ct_Line]
ld d,0
ld e,a
add hl,de ;hl = hl + current_line
ld a,[hl]
ld b,a ;b = current character
ld d,a ;d = current character (serve per il secondo carattere)
ldh a,[Bit_Pos] ;a = Bit_Pos
or a
jr z, .no_shift
.loop:
srl b
dec a
jr nz, .loop
.no_shift:
push hl ;nello stack abbiamo il font read pointer
ld hl,($FF00+Bit_Pos)
ld a,8
sub [hl]
or a
jr z, .noloop
.loop1:
sla d
dec a
jr nz, .loop1
.noloop:
ld a,d
pop de ;in de c'è il font read pointer
pop hl ;in hl c'è il ram write pos. pointer
push de
ld d,a

ld a,b
not a
or [hl]
ld [hl+],a
ld b,a
ldh a,[$91]
or a
jr nz, gbc
ld a,b
ld [hl+],a
jr nogb
gbc:
inc hl
nogb:
ld a,d
ld b,a
pop de
push hl ;ram write pos. pointer
ld d,0
ld e,14
add hl,de
ld a,b
not a
ld [hl+],a
ldh a,[$91]
or a
jr nz, gbc2
ld a,b
ld [hl],a
gbc2:
ld hl, ($FF00+Ct_Line)
inc [hl]
dec c
jp nz, write_loop
ld c,8
xor a
ldh [Ct_Line],a
ldh a, [Tile_Pos]
ld h,0
ld l,a
sla l
rl h
sla l
rl h
sla l
rl h ;tile_pos*8
ldh a,[Bit_Pos]
ld d,0
ld e,a
add hl,de ;tile_pos*8+bit_pos
push hl
ld hl,widthtbl
ldh a,[Charx]
ld d,0
ld e,a
add hl, de
ld l,[hl]
ld h,0
pop de
add hl, de  ;tile_pos*8+bit_pos+char_length
ld a,l
and $07
ldh [Bit_Pos],a
srl h
rr l
srl h
rr l
srl h
rr l ;(tile_pos*8+bit_pos+char_length)/8
ld a,l
ldh [Tile_Pos],a
pop hl
finish:

ld hl,$D3B3
inc [hl]
xor a
ret

calcolo_offset16:
add hl,hl
calcolo_offset8:
add hl,hl
add hl,hl
add hl,hl
add hl, bc  ;in hl c'è il puntatore
ret
