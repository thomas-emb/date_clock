from encoder import Slice, Circle, renderFile, createFootprint
from pathlib import Path

day = [
    [Slice(6, 18, goOut=True), Slice(186, 342, space=True)], # day 29-31 and space after
    Circle(),
    [Slice(93, 180, goIn=True), Slice(273, 180, space=True)] # day 1-30 and space after
]

week = [Circle(),
    [
        Slice(234, 6, goIn=True), Slice(240, 6, space=True), # 1st day and space after
        Slice(246, 6, goIn=True), Slice(252, 6, space=True), # 2nd
        Slice(258, 6, goIn=True), Slice(264, 6, space=True), # 3rd
        Slice(270, 6, goIn=True), Slice(276, 6, space=True), # 4th
        Slice(282, 6, goIn=True), Slice(288, 6, space=True), # 5th
        Slice(294, 6, goIn=True), Slice(300, 6, space=True), # 6th
        Slice(306, 6, goIn=True), Slice( 90, 282, space=True)# 7th day and remaining space
    ]
]

month = [Circle(),
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
]

year = [Circle(), [Slice(90, 90, goIn=True), Slice(-90, 270, space=True)]]

def writeEncoder(name, slices):
    Path("date clock encoders.pretty/"+name+".kicad_mod").write_text(
        renderFile(createFootprint(name, 5, 15, 0.5, 0.15, slices))
    )

if __name__ == '__main__':
    writeEncoder("day", day)
    writeEncoder("week", week)
    writeEncoder("month", month)
    writeEncoder("year", year)