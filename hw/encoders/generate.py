import uuid
from sexpdata import Symbol, dumps as SExpDump # pip install sexpdata
import re
from cmath import rect, phase, pi
from pathlib import Path


def property(name, value, position, layer="F.Fab", hide=None, size = [1, 1], thickness = 0.15, property_name="property"):
    property = [Symbol(property_name), name, str(value),
         [Symbol("at"), position[0], position[1], position[2]],
         [Symbol("unlocked"), True],
         [Symbol("layer"), layer],
         # hide
         [Symbol("uuid"), str(uuid.uuid4())],
         [Symbol("effects"),
            [Symbol("font"),
                [Symbol("size"), size[0], size[1]],
                [Symbol("thickness"), thickness]]]]
    if (hide != None):
        property.insert(6, [Symbol("hide"), hide])
    return property

def rad(degrees):
    return degrees/360 * 2*pi

def polygon(innerRadius, outerRadius, centerAngle, angularWidth, spacing=0):
    angles = [centerAngle - angularWidth/2, centerAngle, centerAngle + angularWidth/2]
    inner = [rect(innerRadius, rad(angles[0])), rect(innerRadius, rad(angles[1])), rect(innerRadius, rad(angles[2]))]
    outer = [rect(outerRadius, rad(angles[2])), rect(outerRadius, rad(angles[1])), rect(outerRadius, rad(angles[0]))]
    if spacing != 0: # spacing is not exact, but good enough for small spacings
        inner[0] = rect(innerRadius, phase(inner[0] + rect(spacing, rad(angles[0] + 90))))
        outer[2] = rect(outerRadius, phase(outer[2] + rect(spacing, rad(angles[0] + 90))))
        inner[2] = rect(innerRadius, phase(inner[2] + rect(spacing, rad(angles[2] - 90))))
        outer[0] = rect(outerRadius, phase(outer[0] + rect(spacing, rad(angles[2] - 90))))
    return [Symbol("fp_poly"),
        [Symbol("pts"),
            [Symbol("arc"),
                [Symbol("start"), round(inner[0].real, 6), -round(inner[0].imag, 6)],
                [Symbol("mid"), round(inner[1].real, 6), -round(inner[1].imag, 6)],
                [Symbol("end"), round(inner[2].real, 6), -round(inner[2].imag, 6)],
            ],
            [Symbol("arc"),
                [Symbol("start"), round(outer[0].real, 6), -round(outer[0].imag, 6)],
                [Symbol("mid"), round(outer[1].real, 6), -round(outer[1].imag, 6)],
                [Symbol("end"), round(outer[2].real, 6), -round(outer[2].imag, 6)],
            ],
        ],
        [Symbol("stroke"),
            [Symbol("width"), 0],
            [Symbol("type"), Symbol("solid")],
        ],
        [Symbol("fill"), Symbol("solid")],
        [Symbol("layer"), "F.Cu"],
        [Symbol("uuid"), str(uuid.uuid4())]
    ]

def clearancePad(hole, size):
    return [
    Symbol("pad"), "", Symbol("np_thru_hole"), Symbol("circle"),
        [Symbol("at"), 0, 0],
        [Symbol("size"), size, size],
        [Symbol("drill"), hole],
        [Symbol("layers"), "*.Mask"],
        [Symbol("remove_unused_layers"), False],
        [Symbol("uuid"), str(uuid.uuid4())]
    ]

class Slice:
    def __init__(self, center, width, space=False, goIn=False, goOut=False):
        self.center = center
        self.width = width
        self.space = space
        self.goIn = goIn
        self.goOut = goOut

def Circle():
    return [Slice(0, 180), Slice(180, 180)]

def createFootprint(name, hole, size, margin, spacing, rings):
    innerRadius = hole/2 + (margin - spacing/2)
    outerRadius = size/2 - (margin - spacing/2)
    ringHeight = (outerRadius - innerRadius)/len(rings)
    radii = [ring*ringHeight + innerRadius for ring in range(len(rings) + 1)]
    result = [
        property("Reference", "P**", [size/2, -size/2, 0], "F.SilkS", thickness=0.1),
        property("Value", name, [size/2, size/2, 0]),
        property("Footprint", "", [0, 0, 0], hide=True),
        property("Datasheet", "", [0, 0, 0], hide=True),
        property("Description", "", [0, 0, 0], hide=True),
        [Symbol("attr"), Symbol("smd")]
    ]
    for ring, slices in enumerate(rings):
        for slice in slices:
            result.append(polygon(
                radii[ring] + (-spacing if slice.goIn else spacing/2),
                radii[ring+1] - (-spacing if slice.goOut else spacing/2),
                slice.center,
                slice.width,
                spacing if slice.space else 0
            ))
    kicad_mod = [
        Symbol("footprint"), name,
        [Symbol("version"), 20240108],
        [Symbol("generator"), "python"],
        [Symbol("generator_version"), "0.1"],
        [Symbol("layer"), "F.Cu"]
    ] + result + [
        property(Symbol("user"), "${REFERENCE}", [-size/2, size/2, 0], property_name="fp_text"),
        clearancePad(hole, size)
    ]
    kicad_mod = SExpDump(kicad_mod, true_as="yes", false_as="no", pretty_print=True, indent_as="\t")
    kicad_mod = re.sub("\n[\t]+(?=[^\t()])", " ", kicad_mod)
    kicad_mod = re.sub("\\( ", "(", kicad_mod)
    return kicad_mod

def writeEncoder(name, slices):
    Path("date clock encoders.pretty/"+name+".kicad_mod").write_text(
        createFootprint(name, 5, 15, 0.5, 0.15, slices)
    )

writeEncoder("day", [
    [Slice(6, 18, goOut=True), Slice(186, 342, space=True)], # day 29-31 and space after
    Circle(),
    [Slice(93, 180, goIn=True), Slice(273, 180, space=True)] # day 1-30 and space after
])

writeEncoder("week", [Circle(),
    [
        Slice(234, 6, goIn=True), Slice(240, 6, space=True), # 1st day and space after
        Slice(246, 6, goIn=True), Slice(252, 6, space=True), # 2nd
        Slice(258, 6, goIn=True), Slice(264, 6, space=True), # 3rd
        Slice(270, 6, goIn=True), Slice(276, 6, space=True), # 4th
        Slice(282, 6, goIn=True), Slice(288, 6, space=True), # 5th
        Slice(294, 6, goIn=True), Slice(300, 6, space=True), # 6th
        Slice(306, 6, goIn=True), Slice( 90, 282, space=True)# 7th day and remaining space
    ]
])

writeEncoder("month", [Circle(),
    [
        Slice(  0, 15, goIn =True), Slice( 15, 15, space=True), # September
        Slice( 30, 15, goOut=True), Slice( 45, 15, space=True), # August
        Slice( 60, 15, goOut=True), Slice( 75, 15, space=True), # July
        Slice( 90, 15, goIn =True), Slice(105, 15, space=True), # June
        Slice(120, 15, goOut=True), Slice(135, 15, space=True), # May
        Slice(150, 15, goIn =True), Slice(165, 15, space=True), # April
        Slice(180, 15, goOut=True),#Slice(195, 15, space=True), # March
        Slice(210, 45, space=True),#Slice(225, 15, space=True), # February
        Slice(240, 15, goOut=True), Slice(255, 15, space=True), # January
        Slice(270, 15, goOut=True), Slice(285, 15, space=True), # December
        Slice(300, 15, goIn =True), Slice(315, 15, space=True), # November
        Slice(330, 15, goOut=True), Slice(345, 15, space=True), # October
    ],
    Circle()
])

writeEncoder("year", [Circle(), [Slice(90, 90, goIn=True), Slice(-90, 270, space=True)]])
