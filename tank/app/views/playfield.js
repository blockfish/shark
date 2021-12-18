const { View } = require('../view');
const theme = require('../theme');
const rules = require('../rules');

const cellSize = 32;

class PlayfieldView extends View {
    constructor() {
        super();
        this.width = cellSize * rules.cols;
        this.height = cellSize * rules.rows;
        console.log('new playfieldview');

        this.cells = {};
        this.ghostCells = {type: 'G', coords: []};
        this.cursorPos = null;
        this.cursorDragging = false;

        this.onDragCursor = undefined;
    }

    setCells(cells) {
        for (let type in this.cells) {
            this.cells[type].length = 0;
        }
        for (let { x, y, type } of cells) {
            (this.cells[type] ||= []).push([x, y]);
        }
        this.requestRepaint();
    }

    setGhost(cells) {
        this.ghostCells.coords.length = 0;
        for (let { x, y, type } of cells) {
            this.ghostCells.type = type;
            this.ghostCells.coords.push([x, y]);
        }
    }

    render(g) {
        super.render(g);

        let { rows, cols } = rules;
        let sz = cellSize;
        let gw = sz * cols, gh = sz * rows;

        // board outline
        g.beginPath();
        g.moveTo(gw - 0.5, 0.5);
        g.lineTo(gw - 0.5, gh - 0.5);
        g.lineTo(0.5, gh - 0.5);
        g.lineTo(0.5, 0.5);
        g.strokeStyle = theme.grid.outer[0];
        g.globalAlpha = theme.grid.outer[1];
        g.lineWidth = theme.grid.outer[2];
        g.stroke();

        // board gridlines
        g.beginPath();
        for (let x = 1; x < cols; x++) {
            g.moveTo(sz * x + 0.5, 0);
            g.lineTo(sz * x + 0.5, gh);
        }
        for (let y = 1; y < rows; y++) {
            g.moveTo(0,  sz * y + 0.5);
            g.lineTo(gw, sz * y + 0.5);
        }
        g.strokeStyle = theme.grid.inner[0];
        g.globalAlpha = theme.grid.inner[1];
        g.lineWidth = theme.grid.inner[2];
        g.stroke();

        // regular cells
        g.globalAlpha = 1;
        for (let type in this.cells) {
            g.beginPath();
            for (let [x, y] of this.cells[type]) {
                g.rect(sz * x, sz * (rules.rows - y - 1), sz, sz);
            }
            g.fillStyle = theme.blocks[type];
            g.fill();
        }

        // ghost cells
        let [ga, glw, gr] = theme.ghost;
        g.beginPath();
        for (let [x, y] of this.ghostCells.coords) {
            g.rect(
                sz * x + gr + glw / 2,
                sz * (rules.rows - y - 1) + gr + glw / 2,
                sz - gr * 2 - glw,
                sz - gr * 2 - glw,
            );
        }
        g.strokeStyle = theme.blocks[this.ghostCells.type];
        g.globalAlpha = ga;
        g.lineWidth = glw;
        g.stroke();

        // cursor
        if (this.cursorPos !== null) {
            let [cx, cy] = this.cursorPos;
            let lw = theme.cursor[this.cursorDragging ? 3 : 2];
            g.save();
            g.translate(sz * cx, sz * (rules.rows - cy - 1));
            g.beginPath();
            g.rect(lw / 2, lw / 2, sz - lw, sz - lw);
            g.strokeStyle = theme.cursor[0];
            g.globalAlpha = theme.cursor[1];
            g.lineWidth = lw;
            g.stroke();
            g.restore()
        }
    }

    update(mouse) {
        super.update(mouse);

        let cx = Math.floor(mouse.x / cellSize);
        let cy = rules.rows - Math.floor(mouse.y / cellSize) - 1;
        let dragType = null;

        if (this.cursorDragging && !mouse.leftBtn) {
            this.cursorDragging = false;
            this.requestRepaint();
            dragType = 'end';
        }

        if (cx < 0 || cx >= rules.cols || cy < 0 || cy >= rules.rows) {
            cx = cy = null;
            if (this.cursorPos !== null) {
                this.cursorPos = null;
                this.pointer = false;
                this.requestRepaint();
            }
        } else {
            let [prevX, prevY] = this.cursorPos || [-1, -1];
            if (cx !== prevX || cy !== prevY) {
                this.cursorPos = [cx, cy];
                this.pointer = true;
                this.requestRepaint();
                if (this.cursorDragging) {
                    dragType = 'drag';
                }
            }
            if (!this.cursorDragging && mouse.leftBtn === 'pressed') {
                this.cursorDragging = true;
                this.requestRepaint();
                dragType = 'start';
            }
        }

        if (this.onDragCursor && dragType !== null) {
            this.onDragCursor(dragType, cx, cy);
        }
    }
}

module.exports = { PlayfieldView };
