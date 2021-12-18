const { View } = require('../view');
const theme = require('../theme');
const rules = require('../rules');

const cellSize = 32;

let $hover = Symbol("hover");
let $selected = Symbol("selcted");

class ColorPadView extends View {
    constructor(type) {
        super();
        this.type = type;
        this.width = cellSize;
        this.height = cellSize;

        this[$hover] = false;
        this[$selected] = false;
        this.onClick = undefined;
        console.log('new colorpadview', type);
    }

    setSelected(selected) {
        if (this[$selected] !== selected) {
            this[$selected] = selected;
            this.requestRepaint();
        }
    }

    render(g) {
        super.render(g);

        // block
        let sz = cellSize;
        g.fillStyle = theme.blocks[this.type];
        g.fillRect(0, 0, sz, sz);

        // border
        if (this[$hover] || this[$selected]) {
            let lw = theme.cursor[this[$selected] ? 3 : 2];
            g.beginPath();
            g.rect(lw / 2, lw / 2, sz - lw, sz - lw);
            g.strokeStyle = theme.cursor[0];
            g.globalAlpha = theme.cursor[1];
            g.lineWidth = lw;
            g.stroke();
        }
    }

    update(mouse) {
        super.update(mouse);
        let over = (mouse.x >= 0 && mouse.x <= cellSize &&
                    mouse.y >= 0 && mouse.y <= cellSize);

        if (this[$hover] && !over) {
            this[$hover] = false;
            this.pointer = false;
            this.requestRepaint();
        } else if (this[$hover] && mouse.leftBtn === 'pressed') {
            if (this.onClick) {
                this.onClick();
            }
        } else if (!this[$hover] && over) {
            this[$hover] = true;
            this.pointer = true;
            this.requestRepaint();
        }
    }
}

ColorPadView.ALL_TYPES = [
    "G", "auto",
    ...Object.keys(rules.shapes)
];

module.exports = { ColorPadView };
