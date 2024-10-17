# https://dev-docs.kicad.org/en/file-formats/sexpr-footprint @2024-06-04
from uuid import uuid4
from sexpdata import Symbol, dumps as SExpDump # pip install sexpdata
from re import sub as regexSubstitute
from cmath import rect, pi
from math import asin, radians

def property(name, value, position, layer="F.Fab", hide=None, size = [1, 1], thickness = 0.15, propertyName="property"):
    property = [Symbol(propertyName), name, str(value),
         [Symbol("at"), position[0], position[1], position[2]],
         [Symbol("unlocked"), True],
         [Symbol("layer"), layer],
         # hide
         [Symbol("uuid"), str(uuid4())],
         [Symbol("effects"),
            [Symbol("font"),
                [Symbol("size"), size[0], size[1]],
                [Symbol("thickness"), thickness]
            ]
        ]
    ]
    if (hide != None):
        property.insert(6, [Symbol("hide"), hide])
    return property

def nice_round(value, digits=0):
    value = round(value, digits)
    return value if value != int(value) else int(value)

def calculatePolygon(innerRadius, outerRadius, centerAngle, angularWidth, spacing=0):
    angles = [centerAngle - angularWidth/2, centerAngle, centerAngle + angularWidth/2]
    innerSpacingAngle = asin(spacing / innerRadius)
    outerSpacingAngle = asin(spacing / outerRadius)
    inner = [
        rect(innerRadius, radians(angles[0]) + innerSpacingAngle),
        rect(innerRadius, radians(angles[1])),
        rect(innerRadius, radians(angles[2]) - innerSpacingAngle)
    ]
    outer = [
        rect(outerRadius, radians(angles[2]) - outerSpacingAngle),
        rect(outerRadius, radians(angles[1])),
        rect(outerRadius, radians(angles[0]) + outerSpacingAngle)
    ]
    return [inner, outer]

def polygon(innerRadius, outerRadius, centerAngle, angularWidth, spacing=0):
    [inner, outer] = calculatePolygon(innerRadius, outerRadius, centerAngle, angularWidth, spacing)
    return [Symbol("fp_poly"),
        [Symbol("pts"),
            [Symbol("arc"),
                [Symbol("start"), nice_round(inner[0].real, 6), -nice_round(inner[0].imag, 6)],
                [Symbol("mid"), nice_round(inner[1].real, 6), -nice_round(inner[1].imag, 6)],
                [Symbol("end"), nice_round(inner[2].real, 6), -nice_round(inner[2].imag, 6)]
            ],
            [Symbol("arc"),
                [Symbol("start"), nice_round(outer[0].real, 6), -nice_round(outer[0].imag, 6)],
                [Symbol("mid"), nice_round(outer[1].real, 6), -nice_round(outer[1].imag, 6)],
                [Symbol("end"), nice_round(outer[2].real, 6), -nice_round(outer[2].imag, 6)]
            ],
        ],
        [Symbol("stroke"),
            [Symbol("width"), 0],
            [Symbol("type"), Symbol("solid")]
        ],
        [Symbol("fill"), Symbol("solid")],
        [Symbol("layer"), "F.Cu"],
        [Symbol("uuid"), str(uuid4())]
    ]

def clearancePad(hole, size):
    return [
    Symbol("pad"), "", Symbol("np_thru_hole"), Symbol("circle"),
        [Symbol("at"), 0, 0],
        [Symbol("size"), size, size],
        [Symbol("drill"), hole],
        [Symbol("layers"), "*.Mask"],
        [Symbol("remove_unused_layers"), False],
        [Symbol("uuid"), str(uuid4())]
    ]

class Slice:
    def __init__(self, center, width, space=False, goIn=False, goOut=False):
        self.center = center
        self.width = width
        self.space = space
        self.goIn = goIn
        self.goOut = goOut

def Circle(rotation = 0):
    return [Slice(rotation, 180), Slice(rotation+180, 180)]

def renderFile(struct):
    kicad_mod = SExpDump(struct, true_as="yes", false_as="no", pretty_print=True, indent_as="\t")
    kicad_mod = regexSubstitute("\n[\t]+(?=[^\t()])", " ", kicad_mod)
    kicad_mod = regexSubstitute("\\( ", "(", kicad_mod)
    return kicad_mod

def createRings(innerRadius, outerRadius, margin, spacing, rings):
    innerRadius += (margin - spacing/2)
    outerRadius -= (margin - spacing/2)
    ringHeight = (outerRadius - innerRadius) / len(rings)
    radii = [ring*ringHeight + innerRadius for ring in range(len(rings) + 1)]
    return [
        polygon(
                radii[ring] + (-spacing if slice.goIn else spacing/2),
                radii[ring+1] - (-spacing if slice.goOut else spacing/2),
                slice.center,
                slice.width,
                spacing if slice.space else 0
        )
        for ring, slices in enumerate(rings)
        for slice in slices
    ]

def createFootprint(name, hole, size, margin, spacing, rings):
    return [
        Symbol("footprint"), name,
        [Symbol("version"), 20240108],
        [Symbol("generator"), "python"],
        [Symbol("generator_version"), "0.1"],
        [Symbol("layer"), "F.Cu"],
        property("Reference", "P**", [size/2, -size/2, 0], "F.SilkS", thickness=0.1),
        property("Value", name, [size/2, size/2, 0]),
        property("Footprint", "", [0, 0, 0], hide=True),
        property("Datasheet", "", [0, 0, 0], hide=True),
        property("Description", "", [0, 0, 0], hide=True),
        [Symbol("attr"), Symbol("smd")]
    ] + createRings(hole/2, size/2, margin, spacing, rings) + [
        property(Symbol("user"), "${REFERENCE}", [-size/2, size/2, 0], propertyName="fp_text"),
        clearancePad(hole, size)
    ]
