""" Python Character Mapping Codec generated from 'TURKISH.TXT' with gencodec.py.

Written by Marc-Andre Lemburg (mal@lemburg.com).

(c) Copyright CNRI, All Rights Reserved. NO WARRANTY.
(c) Copyright 2000 Guido van Rossum.

"""#"

import codecs

### Codec APIs

class Codec(codecs.Codec):

    def encode(self,input,errors='strict'):

        return codecs.charmap_encode(input,errors,encoding_map)

    def decode(self,input,errors='strict'):

        return codecs.charmap_decode(input,errors,decoding_map)

class StreamWriter(Codec,codecs.StreamWriter):
    pass

class StreamReader(Codec,codecs.StreamReader):
    pass

### encodings module API

def getregentry():

    return (Codec().encode,Codec().decode,StreamReader,StreamWriter)

### Decoding Map

decoding_map = codecs.make_identity_dict(range(256))
decoding_map.update({
        0x0080: 0x00c4, # LATIN CAPITAL LETTER A WITH DIAERESIS
        0x0081: 0x00c5, # LATIN CAPITAL LETTER A WITH RING ABOVE
        0x0082: 0x00c7, # LATIN CAPITAL LETTER C WITH CEDILLA
        0x0083: 0x00c9, # LATIN CAPITAL LETTER E WITH ACUTE
        0x0084: 0x00d1, # LATIN CAPITAL LETTER N WITH TILDE
        0x0085: 0x00d6, # LATIN CAPITAL LETTER O WITH DIAERESIS
        0x0086: 0x00dc, # LATIN CAPITAL LETTER U WITH DIAERESIS
        0x0087: 0x00e1, # LATIN SMALL LETTER A WITH ACUTE
        0x0088: 0x00e0, # LATIN SMALL LETTER A WITH GRAVE
        0x0089: 0x00e2, # LATIN SMALL LETTER A WITH CIRCUMFLEX
        0x008a: 0x00e4, # LATIN SMALL LETTER A WITH DIAERESIS
        0x008b: 0x00e3, # LATIN SMALL LETTER A WITH TILDE
        0x008c: 0x00e5, # LATIN SMALL LETTER A WITH RING ABOVE
        0x008d: 0x00e7, # LATIN SMALL LETTER C WITH CEDILLA
        0x008e: 0x00e9, # LATIN SMALL LETTER E WITH ACUTE
        0x008f: 0x00e8, # LATIN SMALL LETTER E WITH GRAVE
        0x0090: 0x00ea, # LATIN SMALL LETTER E WITH CIRCUMFLEX
        0x0091: 0x00eb, # LATIN SMALL LETTER E WITH DIAERESIS
        0x0092: 0x00ed, # LATIN SMALL LETTER I WITH ACUTE
        0x0093: 0x00ec, # LATIN SMALL LETTER I WITH GRAVE
        0x0094: 0x00ee, # LATIN SMALL LETTER I WITH CIRCUMFLEX
        0x0095: 0x00ef, # LATIN SMALL LETTER I WITH DIAERESIS
        0x0096: 0x00f1, # LATIN SMALL LETTER N WITH TILDE
        0x0097: 0x00f3, # LATIN SMALL LETTER O WITH ACUTE
        0x0098: 0x00f2, # LATIN SMALL LETTER O WITH GRAVE
        0x0099: 0x00f4, # LATIN SMALL LETTER O WITH CIRCUMFLEX
        0x009a: 0x00f6, # LATIN SMALL LETTER O WITH DIAERESIS
        0x009b: 0x00f5, # LATIN SMALL LETTER O WITH TILDE
        0x009c: 0x00fa, # LATIN SMALL LETTER U WITH ACUTE
        0x009d: 0x00f9, # LATIN SMALL LETTER U WITH GRAVE
        0x009e: 0x00fb, # LATIN SMALL LETTER U WITH CIRCUMFLEX
        0x009f: 0x00fc, # LATIN SMALL LETTER U WITH DIAERESIS
        0x00a0: 0x2020, # DAGGER
        0x00a1: 0x00b0, # DEGREE SIGN
        0x00a4: 0x00a7, # SECTION SIGN
        0x00a5: 0x2022, # BULLET
        0x00a6: 0x00b6, # PILCROW SIGN
        0x00a7: 0x00df, # LATIN SMALL LETTER SHARP S
        0x00a8: 0x00ae, # REGISTERED SIGN
        0x00aa: 0x2122, # TRADE MARK SIGN
        0x00ab: 0x00b4, # ACUTE ACCENT
        0x00ac: 0x00a8, # DIAERESIS
        0x00ad: 0x2260, # NOT EQUAL TO
        0x00ae: 0x00c6, # LATIN CAPITAL LIGATURE AE
        0x00af: 0x00d8, # LATIN CAPITAL LETTER O WITH STROKE
        0x00b0: 0x221e, # INFINITY
        0x00b2: 0x2264, # LESS-THAN OR EQUAL TO
        0x00b3: 0x2265, # GREATER-THAN OR EQUAL TO
        0x00b4: 0x00a5, # YEN SIGN
        0x00b6: 0x2202, # PARTIAL DIFFERENTIAL
        0x00b7: 0x2211, # N-ARY SUMMATION
        0x00b8: 0x220f, # N-ARY PRODUCT
        0x00b9: 0x03c0, # GREEK SMALL LETTER PI
        0x00ba: 0x222b, # INTEGRAL
        0x00bb: 0x00aa, # FEMININE ORDINAL INDICATOR
        0x00bc: 0x00ba, # MASCULINE ORDINAL INDICATOR
        0x00bd: 0x2126, # OHM SIGN
        0x00be: 0x00e6, # LATIN SMALL LIGATURE AE
        0x00bf: 0x00f8, # LATIN SMALL LETTER O WITH STROKE
        0x00c0: 0x00bf, # INVERTED QUESTION MARK
        0x00c1: 0x00a1, # INVERTED EXCLAMATION MARK
        0x00c2: 0x00ac, # NOT SIGN
        0x00c3: 0x221a, # SQUARE ROOT
        0x00c4: 0x0192, # LATIN SMALL LETTER F WITH HOOK
        0x00c5: 0x2248, # ALMOST EQUAL TO
        0x00c6: 0x2206, # INCREMENT
        0x00c7: 0x00ab, # LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
        0x00c8: 0x00bb, # RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
        0x00c9: 0x2026, # HORIZONTAL ELLIPSIS
        0x00ca: 0x00a0, # NO-BREAK SPACE
        0x00cb: 0x00c0, # LATIN CAPITAL LETTER A WITH GRAVE
        0x00cc: 0x00c3, # LATIN CAPITAL LETTER A WITH TILDE
        0x00cd: 0x00d5, # LATIN CAPITAL LETTER O WITH TILDE
        0x00ce: 0x0152, # LATIN CAPITAL LIGATURE OE
        0x00cf: 0x0153, # LATIN SMALL LIGATURE OE
        0x00d0: 0x2013, # EN DASH
        0x00d1: 0x2014, # EM DASH
        0x00d2: 0x201c, # LEFT DOUBLE QUOTATION MARK
        0x00d3: 0x201d, # RIGHT DOUBLE QUOTATION MARK
        0x00d4: 0x2018, # LEFT SINGLE QUOTATION MARK
        0x00d5: 0x2019, # RIGHT SINGLE QUOTATION MARK
        0x00d6: 0x00f7, # DIVISION SIGN
        0x00d7: 0x25ca, # LOZENGE
        0x00d8: 0x00ff, # LATIN SMALL LETTER Y WITH DIAERESIS
        0x00d9: 0x0178, # LATIN CAPITAL LETTER Y WITH DIAERESIS
        0x00da: 0x011e, # LATIN CAPITAL LETTER G WITH BREVE
        0x00db: 0x011f, # LATIN SMALL LETTER G WITH BREVE
        0x00dc: 0x0130, # LATIN CAPITAL LETTER I WITH DOT ABOVE
        0x00dd: 0x0131, # LATIN SMALL LETTER DOTLESS I
        0x00de: 0x015e, # LATIN CAPITAL LETTER S WITH CEDILLA
        0x00df: 0x015f, # LATIN SMALL LETTER S WITH CEDILLA
        0x00e0: 0x2021, # DOUBLE DAGGER
        0x00e1: 0x00b7, # MIDDLE DOT
        0x00e2: 0x201a, # SINGLE LOW-9 QUOTATION MARK
        0x00e3: 0x201e, # DOUBLE LOW-9 QUOTATION MARK
        0x00e4: 0x2030, # PER MILLE SIGN
        0x00e5: 0x00c2, # LATIN CAPITAL LETTER A WITH CIRCUMFLEX
        0x00e6: 0x00ca, # LATIN CAPITAL LETTER E WITH CIRCUMFLEX
        0x00e7: 0x00c1, # LATIN CAPITAL LETTER A WITH ACUTE
        0x00e8: 0x00cb, # LATIN CAPITAL LETTER E WITH DIAERESIS
        0x00e9: 0x00c8, # LATIN CAPITAL LETTER E WITH GRAVE
        0x00ea: 0x00cd, # LATIN CAPITAL LETTER I WITH ACUTE
        0x00eb: 0x00ce, # LATIN CAPITAL LETTER I WITH CIRCUMFLEX
        0x00ec: 0x00cf, # LATIN CAPITAL LETTER I WITH DIAERESIS
        0x00ed: 0x00cc, # LATIN CAPITAL LETTER I WITH GRAVE
        0x00ee: 0x00d3, # LATIN CAPITAL LETTER O WITH ACUTE
        0x00ef: 0x00d4, # LATIN CAPITAL LETTER O WITH CIRCUMFLEX
        0x00f0: None,   # UNDEFINED
        0x00f1: 0x00d2, # LATIN CAPITAL LETTER O WITH GRAVE
        0x00f2: 0x00da, # LATIN CAPITAL LETTER U WITH ACUTE
        0x00f3: 0x00db, # LATIN CAPITAL LETTER U WITH CIRCUMFLEX
        0x00f4: 0x00d9, # LATIN CAPITAL LETTER U WITH GRAVE
        0x00f5: None,   # UNDEFINED
        0x00f6: 0x02c6, # MODIFIER LETTER CIRCUMFLEX ACCENT
        0x00f7: 0x02dc, # SMALL TILDE
        0x00f8: 0x00af, # MACRON
        0x00f9: 0x02d8, # BREVE
        0x00fa: 0x02d9, # DOT ABOVE
        0x00fb: 0x02da, # RING ABOVE
        0x00fc: 0x00b8, # CEDILLA
        0x00fd: 0x02dd, # DOUBLE ACUTE ACCENT
        0x00fe: 0x02db, # OGONEK
        0x00ff: 0x02c7, # CARON
})

### Encoding Map

encoding_map = codecs.make_encoding_map(decoding_map)
