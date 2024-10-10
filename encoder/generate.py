import uuid
from sexpdata import Symbol, dumps as SExpDump # pip install sexpdata
import re
import cmath

symbolName = "year"

unlocked = [Symbol("unlocked"), True]

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

def deg(degrees):
    return degrees/360 * cmath.pi * 2

def polygon(inner_radius, outer_radius, center_angle, angular_width):
    inner = [
        cmath.rect(inner_radius, deg(center_angle - angular_width/2)),
        cmath.rect(inner_radius, deg(center_angle)),
        cmath.rect(inner_radius, deg(center_angle + angular_width/2))]
    outer = [
        cmath.rect(outer_radius, deg(center_angle + angular_width/2)),
        cmath.rect(outer_radius, deg(center_angle)),
        cmath.rect(outer_radius, deg(center_angle - angular_width/2))]
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

def pad(hole, size):
    return [
    Symbol("pad"), "", Symbol("thru_hole"), Symbol("circle"),
        [Symbol("at"), 0, 0],
        [Symbol("size"), size, size],
        [Symbol("drill"), hole],
        [Symbol("layers"), "*.Mask"],
        [Symbol("remove_unused_layers"), False],
        [Symbol("uuid"), str(uuid.uuid4())]
    ]

def items(hole, size, margin, spacing):
    result = [
    property("Reference", "P**", [7, -6.5, 0], "F.SilkS", thickness=0.1),
    property("Value", symbolName, [0, 1, 0]),
    property("Footprint", "", [0, 0, 0], hide=True),
    property("Datasheet", "", [0, 0, 0], hide=True),
    property("Description", "", [0, 0, 0], hide=True),
    [Symbol("attr"), Symbol("smd")]]
    result.append(polygon(5.16, size/2 - margin, -90, 266))
    result.append(polygon(5, size/2 - margin,  90,  90))
    result.append(polygon(hole/2 + margin, 5, 0, 180))
    result.append(polygon(3, 5, 180, 180))

    kicad_mod = [
        Symbol("footprint"), symbolName,
        [Symbol("version"), 20240108],
        [Symbol("generator"), "python"],
        [Symbol("generator_version"), "0.1"],
        [Symbol("layer"), "F.Cu"]
    ] + result + [
        property(Symbol("user"), "${REFERENCE}", [0, 4, 0], property_name="fp_text"),
        pad(hole, size)
    ]

    kicad_mod = SExpDump(kicad_mod, true_as="yes", false_as="no", pretty_print=True, indent_as="\t")
    kicad_mod = re.sub("\n[\t]+(?=[^\t()])", " ", kicad_mod)
    kicad_mod = re.sub("\\( ", "(", kicad_mod)
    return kicad_mod

print(items(5, 15, 0.5, 0.15))