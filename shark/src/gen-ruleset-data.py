import json
import sys
import time

def rotations(xy):
    [x, y] = xy
    return [
        # north
        [x, y],
        # east
        [y, -x],
        # south
        [-x, -y],
        # west
        [-y, x],
    ]


class NormalizedCells:
    def __init__(self, coords):
        xmin = min(x for [x, y] in coords)
        ymin = min(y for [x, y] in coords)
        xmax = max(x for [x, y] in coords)
        ymax = max(y for [x, y] in coords)

        self.x_ofs = xmin
        self.y_ofs = ymin
        self.w = xmax - xmin + 1
        self.h = ymax - ymin + 1

        rows = [0 for i in range(self.h)]
        for [cx, cy] in coords:
            y = cy - ymin
            x = cx - xmin
            rows[y] |= 1 << x
        self.rows = tuple(rows)


class NormalizedShape:
    def __init__(self, coords):
        # rotated[r] : NormalizedCells
        self.rotated = [
            NormalizedCells(coords_r)
            for coords_r in zip(*(rotations(c) for c in coords))
        ]


class ComputedKicks:
    def __init__(self, offsets):
        self.kicks = {}
        for r0 in range(4):
            for dr in [-1, +1]:
                r1 = (r0 + dr + 4) % 4
                self._compute_kicks(offsets, r0, r1)

    def _compute_kicks(self, offsets, r0, r1):
        offs0 = offsets[r0]
        offs1 = offsets[r1]
        self.kicks[(r0, r1)] = []
        for i in range(len(offs0)):
            [x0, y0] = offs0[i]
            [x1, y1] = offs1[i]
            x_ofs = x0 - x1
            y_ofs = y0 - y1
            self.kicks[(r0, r1)].append((x_ofs, y_ofs))


def generate_cells(out, shapes):
    types = sorted(shapes.keys())

    data_rows = []
    rows_index = {}
    for ty in types:
        for r in range(4):
            c = shapes[ty].rotated[r]
            if c.rows not in rows_index:
                rows_index[c.rows] = len(data_rows)
                data_rows += c.rows

    out.write('blockfish::rowbits data_rows[] = {{{}}};\n'.format(
        ','.join(str(r) for r in data_rows)
    ))
    out.write('blockfish::move::ruleset::cells_result data_cells[4*128];\n')
    out.write('void init_cells() {\n')
    for ty in types:
        for r in range(4):
            out.write('/* {} rotation {} */\n'.format(ty, r))
            c = shapes[ty].rotated[r]
            ofs = ord(ty) * 4 + r
            out.write('data_cells[{}].rows = &data_rows[{}];\n'.format(ofs, rows_index[c.rows]))
            out.write('data_cells[{}].x_ofs = {};\n'.format(ofs, c.x_ofs))
            out.write('data_cells[{}].y_ofs = {};\n'.format(ofs, c.y_ofs))
            out.write('data_cells[{}].w = {};\n'.format(ofs, c.w))
            out.write('data_cells[{}].h = {};\n'.format(ofs, c.h))
    out.write('}\n\n')


def generate_kicks(out, kick_table):
    kick_types = sorted(kick_table.keys())

    data_kicks = []
    index_ranges = {}

    for tys in kick_types:
        for (r0, r1), rot_kicks in kick_table[tys].kicks.items():
            begin_idx = len(data_kicks)
            data_kicks += rot_kicks
            end_idx = len(data_kicks)
            for ty in tys:
                index_ranges[(ty, r0, r1)] = (begin_idx, end_idx)

    out.write('blockfish::move::kick data_kicks[] = {{{}}};\n'.format(
        ','.join(f'{{{x_ofs},{y_ofs}}}' for x_ofs, y_ofs in data_kicks)
    ))
    out.write('const blockfish::move::kick* data_kicks_begin[16*128];\n')
    out.write('const blockfish::move::kick* data_kicks_end[16*128];\n')
    out.write('void init_kicks() {\n')
    for (ty, r0, r1), (begin_idx, end_idx) in index_ranges.items():
        ofs = ord(ty) * 16 + r0 * 4 + r1
        out.write('data_kicks_begin[{}] = &data_kicks[{}];\n'.format(ofs, begin_idx))
        out.write('data_kicks_end[{}] = &data_kicks[{}];\n'.format(ofs, end_idx))
    out.write('}\n')


def main(out, ruleset):
    out.write('/' * 90 + '\n')
    out.write('// This file was automatically generated!\n')
    out.write('/' * 90 + '\n\n')

    out.write('#include "blockfish/move.h"\n\n')

    out.write('namespace {\n')

    generate_cells(out, {
        ty: NormalizedShape(coords)
        for ty, coords in ruleset["shapes"].items()
    })

    generate_kicks(out, {
        tys: ComputedKicks(offsets)
        for tys, offsets in ruleset["kicks"].items()
    })

    out.write('}\n\n')

    out.write('namespace blockfish::move {\n')
    out.write('ruleset ruleset::srs() {\n')
    out.write('  init_cells();\n')
    out.write('  init_kicks();\n')
    out.write('  ruleset r;\n')
    out.write('  r.cells_ = data_cells;\n')
    out.write('  r.kicks_begin_ = data_kicks_begin;\n')
    out.write('  r.kicks_end_ = data_kicks_end;\n')
    out.write('  return r;\n')
    out.write('}\n')
    out.write('}\n\n')

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("usage: ruleset-codegen <ruleset.json> <output.cc>")
        sys.exit(1)
    with open(sys.argv[1], 'r') as ruleset_file:
        ruleset = json.load(ruleset_file)
    with open(sys.argv[2], 'w') as output_file:
        main(output_file, ruleset)
