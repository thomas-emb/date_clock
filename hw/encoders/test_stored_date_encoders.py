from pathlib import Path
import create_date_encoders as encoder
from sexpdata import loads
from itertools import filterfalse
import re

drop_uuid = re.compile(r"(?<=uuid \")[^\"]*(?=\"\))")

def ignore_uuid(string):
    return drop_uuid.sub("X", string)

def run_test(name):
    result = loads(ignore_uuid(encoder.renderFile(encoder.createFootprint(name, 5, 15, 0.5, 0.15, getattr(encoder, name)))))
    expect = loads(ignore_uuid(Path("date clock encoders.pretty/"+name+".kicad_mod").read_text()))
    assert expect == result

def test_day():
    run_test("day")

def test_week():
    run_test("week")

def test_month():
    run_test("month")

def test_year():
    run_test("year")