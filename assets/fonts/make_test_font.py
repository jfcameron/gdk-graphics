#!/usr/bin/env python3
"""Generates gdk-test-font.ttf and gdk-test-font.ttc
Note that this script isnt required by the generation or build systems, but its committed in case 
either test fonts ever need to be modified to include new codepoints.
"""

import struct, os

UNITS_PER_EM = 1000
ASCENDER     = 800
DESCENDER    = -200
LINE_GAP     = 100

def be16(v): return struct.pack('>H', v & 0xFFFF)
def sbe16(v): return struct.pack('>h', v)
def be32(v): return struct.pack('>I', v & 0xFFFFFFFF)

def codepoints():
    out = []
    out += [ord(' ')]
    out += [ord(c) for c in "0123456789"]
    out += [ord(c) for c in "-:.,!?'"]
    out += [ord(c) for c in "abcdefghijklmnopqrstuvwxyz"]
    out += [ord(c) for c in "ABCDEFGHIJKLMNOPQRSTUVWXYZ"]
    out += [ord(c) for c in "あいうえおかきくけこ"] 
    out += [ord(c) for c in "日本語一二三四五六七八九十人山川水火木金"] 
    out += [0x20000] # CJK extension B, above the BMP

    return sorted(set(out))

CODEPOINTS = codepoints()

def is_wide(cp):
    """full width, as CJK and kana are in a real font"""
    return cp >= 0x3040

def advance_for(cp):
    if cp == ord(' '):  return 400
    if is_wide(cp):     return UNITS_PER_EM
    if cp < 0x80 and chr(cp).isdigit(): return 600
    if cp < 0x80 and chr(cp).isupper(): return 700
    return 400 + (cp % 5) * 60

def box_for(cp):
    """the rectangle drawn for a codepoint, as (xMin, yMin, xMax, yMax), or None for a blank"""
    if cp == ord(' '): return None
    adv = advance_for(cp)
    side = 60 if not is_wide(cp) else 40
    top = ASCENDER - 100 if not is_wide(cp) else ASCENDER
    bottom = 0 if cp not in (ord('g'), ord('y'), ord('p'), ord('q'), ord('j')) else DESCENDER + 50
    return (side, bottom, adv - side, top)

def simple_glyph(contours):
    """contours: list of lists of (x, y) on-curve points"""
    if not contours: return b''
    xs = [p[0] for c in contours for p in c]
    ys = [p[1] for c in contours for p in c]
    out = sbe16(len(contours)) + sbe16(min(xs)) + sbe16(min(ys)) + sbe16(max(xs)) + sbe16(max(ys))
    end = -1
    for c in contours:
        end += len(c)
        out += be16(end)
    out += be16(0)                                
    points = [p for c in contours for p in c]
    out += bytes([0x01]) * len(points)             
    prev = 0
    for x, _ in points:
        out += sbe16(x - prev); prev = x
    prev = 0
    for _, y in points:
        out += sbe16(y - prev); prev = y
    return out

def rect(x0, y0, x1, y1):
    return [(x0, y0), (x1, y0), (x1, y1), (x0, y1)]

def notdef_glyph():
    outer = rect(60, 0, 540, 700)
    inner = list(reversed(rect(120, 60, 480, 640)))
    return simple_glyph([outer, inner])

def glyph_for(cp):
    box = box_for(cp)
    if box is None: return b''               
    return simple_glyph([rect(*box)])

TALL_ASCENDER  = 1100
TALL_DESCENDER = -300

def tables_for(ascender=ASCENDER, descender=DESCENDER):
    """every table of the font, with the vertical metrics given; the glyphs never change"""
    glyphs = [notdef_glyph()] + [glyph_for(cp) for cp in CODEPOINTS]
    advances = [600] + [advance_for(cp) for cp in CODEPOINTS]
    lsbs = [60] + [(box_for(cp)[0] if box_for(cp) else 0) for cp in CODEPOINTS]
    num_glyphs = len(glyphs)

    glyf = b''; loca = []
    for g in glyphs:
        loca.append(len(glyf))
        glyf += g + (b'\0' * ((4 - len(g) % 4) % 4))
    loca.append(len(glyf))

    tables = {}
    tables[b'glyf'] = glyf
    tables[b'loca'] = b''.join(be32(o) for o in loca)          
    tables[b'maxp'] = be32(0x00010000) + be16(num_glyphs) + b'\0' * 26
    tables[b'hmtx'] = b''.join(be16(a) + sbe16(l) for a, l in zip(advances, lsbs))
    tables[b'hhea'] = (be32(0x00010000) + sbe16(ascender) + sbe16(descender) + sbe16(LINE_GAP)
        + be16(max(advances)) + sbe16(0) + sbe16(0) + sbe16(0)
        + sbe16(1) + sbe16(0) + sbe16(0) + sbe16(0) * 4 + sbe16(0) + be16(num_glyphs))
    tables[b'head'] = (be32(0x00010000) + be32(0x00010000) + be32(0) + be32(0x5F0F3CF5)
        + be16(0) + be16(UNITS_PER_EM) + b'\0' * 16
        + sbe16(0) + sbe16(descender) + sbe16(UNITS_PER_EM) + sbe16(ascender)
        + be16(0) + be16(8) + sbe16(2) + sbe16(1) + sbe16(0))   

    groups = b''.join(be32(cp) + be32(cp) + be32(i + 1) for i, cp in enumerate(CODEPOINTS))
    sub12 = (be16(12) + be16(0) + be32(16 + len(groups)) + be32(0)
        + be32(len(CODEPOINTS)) + groups)
    tables[b'cmap'] = be16(0) + be16(1) + be16(3) + be16(10) + be32(12) + sub12

    def gid(ch):
        return CODEPOINTS.index(ord(ch)) + 1
    raw_pairs = sorted([(gid('A'), gid('V'), -80), (gid('T'), gid('o'), -50)])
    pairs = b''.join(be16(l) + be16(r) + sbe16(v) for l, r, v in raw_pairs)
    npairs = len(raw_pairs)
    subtable = be16(npairs) + be16(0) + be16(0) + be16(0) + pairs
    tables[b'kern'] = (be16(0) + be16(1)
        + be16(0) + be16(6 + len(subtable)) + be16(1) + subtable)

    return tables

def offset_table(tables, first_table_at):
    """the sfnt header and table directory for one face, whose tables start at first_table_at and
    follow one another in tag order, each padded to four bytes. Offsets are from the start of the
    file, which for a single font is the start of this table and for a collection is not."""
    tags = sorted(tables)
    n = len(tags)
    entry_selector = max(0, (n).bit_length() - 1)
    search_range = (2 ** entry_selector) * 16
    header = (be32(0x00010000) + be16(n) + be16(search_range) + be16(entry_selector)
        + be16(n * 16 - search_range))

    offset = first_table_at
    directory = b''; body = b''
    for tag in tags:
        data = tables[tag]
        padded = data + b'\0' * ((4 - len(data) % 4) % 4)
        checksum = sum(struct.unpack('>%dI' % (len(padded) // 4), padded)) & 0xFFFFFFFF
        directory += tag + be32(checksum) + be32(offset) + be32(len(data))
        body += padded
        offset += len(padded)
    return header + directory, body

def single(tables):
    directory_size = 12 + 16 * len(tables)
    header, body = offset_table(tables, directory_size)
    return header + body

def collection(faces):
    """a TrueType collection: a ttcf header listing each face's offset table, then the offset
    tables, then every face's tables. Nothing is shared between faces, which the format allows
    but does not require."""
    header_size = 12 + 4 * len(faces)
    directories_size = sum(12 + 16 * len(t) for t in faces)

    directories, bodies, offsets = b'', b'', []
    at = header_size + directories_size
    for tables in faces:
        offsets.append(header_size + len(directories))
        directory, body = offset_table(tables, at)
        directories += directory; bodies += body
        at += len(body)

    header = b'ttcf' + be16(1) + be16(0) + be32(len(faces)) + b''.join(be32(o) for o in offsets)
    return header + directories + bodies

def build():
    """the single font, unchanged by the collection's being added"""
    return single(tables_for())

if __name__ == '__main__':
    here = os.path.dirname(os.path.abspath(__file__))

    out = os.path.join(here, 'gdk-test-font.ttf')
    data = build()
    with open(out, 'wb') as f: f.write(data)
    print(f"wrote {out}: {len(data)} bytes, {len(CODEPOINTS) + 1} glyphs")

    out = os.path.join(here, 'gdk-test-font.ttc')
    data = collection([tables_for(), tables_for(TALL_ASCENDER, TALL_DESCENDER)])
    with open(out, 'wb') as f: f.write(data)
    print(f"wrote {out}: {len(data)} bytes, 2 faces: ascent {ASCENDER} and {TALL_ASCENDER}")

