const rules = require('./rules');
let rows = rules.rows, cols = rules.cols;

class Matrix {
    constructor(cells) {
        this.cells = cells;
    }

    getCell(x, y) {
        if (x < 0 || x >= cols || y < 0) {
            return 'G';
        }
        let i = x + y * cols;
        return i < this.cells.length ? Board.COLORS[this.cells[i]] : null;
    }

    *getFilledCells() {
        for (let i = 0; i < this.cells.length; i++) {
            let val = this.cells[i];
            if (val === 0) {
                continue;
            }
            let x = i % cols;
            let y = (i - x) / cols;
            yield { x, y, type: Matrix.COLORS[val] };
        }
    }

    setCells(cells) {
        // XXX(iitalics): lazily init the new board
        let newBoard = null;
        for (let {x, y, type} of cells) {
            let val = Matrix.COLORS.indexOf(type);
            if (val < 0) {
                throw new Error(`invalid cell type ${type}`);
            }

            let i = x + y * cols;
            if (x < 0 || x >= cols || y < 0 || y >= rows) {
                continue;
            }

            if (!newBoard) {
                if (this.cells[i] === val) {
                    // ignore changes with no effect
                    continue;
                }
                newBoard = new Matrix(new Uint8Array(this.cells));
            }
            newBoard.cells[i] = val;
        }
        // if no new board was even created (no new cells were set) then return this
        return newBoard || this;
    }
}

Matrix.COLORS = [
    null,
    "G",
    ...Object.keys(rules.shapes)
];

Matrix.EMPTY = new Matrix(new Uint8Array(rules.cols * rules.rows));

module.exports = { Matrix };
