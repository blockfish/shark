const { View } = require('../view');
const theme = require('../theme');
const rules = require('../rules');

const cellSize = 32;
const padding = 10;
const queuePieceCols = 4;
const queuePieceRows = 2;

const blinkOnTime = 400;
const blinkOffTime = 400;

// XXX(iitalics): compute offset so that pieces will appear centered in the queue
const queuePieceShapeOffset = {};
for (let type in rules.shapes) {
    let minX = queuePieceCols, minY = queuePieceRows;
    let maxX = -queuePieceCols, maxY = -queuePieceRows;
    for (let [x, y] of rules.shapes[type]) {
        minX = Math.min(minX, x);
        minY = Math.min(minY, y);
        maxX = Math.max(maxX, x);
        maxY = Math.max(maxY, y);
    }
    let dx = (queuePieceCols - maxX - minX - 1) / 2;
    let dy = (queuePieceRows + maxY + minY - 1) / 2;
    queuePieceShapeOffset[type] = [dx, dy];
}

let $hover = Symbol("hover");
let $pieces = Symbol("pieces");
let $editing = Symbol("editing");
let $blinkShow = Symbol("blinkShow");
let $blinkTimer = Symbol("blinkTimer");

class QueueView extends View {
    constructor(n) {
        super();
        this.width = cellSize * queuePieceCols + padding * 2;
        this.height = (cellSize * queuePieceRows + padding) * n + padding;
        this[$pieces] = new Array(n);
        for (let i = 0; i < n; i++) {
            this[$pieces][i] = null;
        }

        this[$hover] = false;
        this[$editing] = null;
        this[$blinkShow] = true;
        this[$blinkTimer] = null;
        this.onClick = undefined;
    }

    setPiece(idx, type) {
        if (this[$pieces][idx] !== type) {
            this[$pieces][idx] = type;
            this.requestRepaint();
        }
    }

    setEditing(idx) {
        if (this[$editing] !== idx) {
            this[$editing] = idx;
            this[$blinkShow] = true;
            if (this[$blinkTimer] !== null) {
                clearTimeout(this[$blinkTimer]);
                this[$blinkTimer] = null;
            }
            this.requestRepaint();
        }
    }

    render(g) {
        super.render(g);
        let sz = cellSize;
        let pw = cellSize * queuePieceCols;
        let ph = cellSize * queuePieceRows;

        // pieces
        g.save();
        g.translate(padding, padding);
        g.globalAlpha = 1;
        for (let i = 0; i < this[$pieces].length; i++) {
            let type = this[$pieces][i];

            // piece cells
            if (type !== null) {
                g.beginPath();
                let [dx, dy] = queuePieceShapeOffset[type];
                for (let [x, y] of rules.shapes[type]) {
                    g.rect(sz * (x + dx), sz * (dy - y), sz, sz);
                }
                g.fillStyle = theme.blocks[type];
                g.fill();
            }
            
            // editing outline
            // TODO(iitalics): blinky
            if (this[$editing] === i && this[$blinkShow]) {
                let lw = theme.cursor[3];
                g.beginPath();
                g.rect(lw / 2, lw / 2, pw - lw, ph - lw);
                g.strokeStyle = theme.cursor[0];
                g.globalAlpha = theme.cursor[1];
                g.lineWidth = lw;
                g.stroke();
            }

            g.translate(0, ph + padding);
        }
        g.restore();

        // hover outline
        if (this[$hover]) {
            let lw = theme.cursor[2];
            g.beginPath();
            g.rect(lw / 2, lw / 2, this.width - lw, this.height - lw);
            g.strokeStyle = theme.cursor[0];
            g.globalAlpha = theme.cursor[1];
            g.lineWidth = lw;
            g.stroke();
        }

        if (this[$editing] !== null && this[$blinkTimer] === null) {
            this[$blinkTimer] = setTimeout(() => {
                this[$blinkShow] = !this[$blinkShow];
                this[$blinkTimer] = null;
                this.requestRepaint();
            }, this[$blinkShow] ? blinkOnTime : blinkOffTime);
        }
    }

    update(mouse) {
        let over = (mouse.x >= 0 && mouse.x <= this.width &&
                    mouse.y >= 0 && mouse.y <= this.height);
        if (this[$hover] !== over) {
            this[$hover] = over;
            this.pointer = over;
            this.requestRepaint();
        }

        if (over && mouse.leftBtn === 'pressed') {
            if (this.onClick) {
                this.onClick();
            }
        }
    }
}

module.exports = { QueueView };
