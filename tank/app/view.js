const $unbind = Symbol("unbind");
const $graphics = Symbol("graphics");
const $mouse = Symbol("mouse");
const $repaintRequested = Symbol("repaintRequested");
const $onWindowEvent = Symbol("onWindowEvent");

class View {
    constructor() {
        this[$unbind] = null;
        this[$graphics] = null;
        this[$repaintRequested] = false;
        this[$mouse] = {x: -Infinity, y: -Infinity, leftBtn: false};

        this.width = 66;
        this.height = 67;
        this.pointer = false;
    }

    bind(canvas) {
        if (this[$unbind]) {
            this[$unbind]();
            this[$unbind] = null;
        }
        if (!canvas) {
            return;
        }

        canvas.width = this.width;
        canvas.height = this.height;
        this[$graphics] = canvas.getContext('2d');
        this.requestRepaint();

        let eventCallback = this[$onWindowEvent].bind(this);
        window.addEventListener('mousedown', eventCallback);
        window.addEventListener('mousemove', eventCallback);
        window.addEventListener('mouseup', eventCallback);

        this[$unbind] = () => {
            window.removeEventListener('mousedown', eventCallback);
            window.removeEventListener('mousemove', eventCallback);
            window.removeEventListener('mouseup', eventCallback);
            this[$graphics] = null;
        };
    }

    requestRepaint() {
        let g = this[$graphics];
        if (g && !this[$repaintRequested]) {
            this[$repaintRequested] = true;
            requestAnimationFrame(() => {
                this[$repaintRequested] = false;
                this.render(g);
            });
        }
    }

    [$onWindowEvent](evt) {
        if (!this[$graphics]) {
            return;
        }

        switch (evt.type) {
        case 'mousedown':
            if (evt.button === 0) {
                // XXX(iitalics): 'pressed' will only be seen for a single update
                this[$mouse].leftBtn = 'pressed';
            }
            break;
        case 'mouseup':
            if (evt.button === 0) {
                this[$mouse].leftBtn = null;
            }
            break;
        }

        switch (evt.type) {
        case 'mousemove':
        case 'mousedown':
        case 'mouseup':
            {
                let rect = this[$graphics].canvas.getBoundingClientRect();
                this[$mouse].x = evt.clientX - rect.x;
                this[$mouse].y = evt.clientY - rect.y;
                this.update(this[$mouse]);
            }
            break;
        }

        if (this[$mouse].leftBtn === 'pressed') {
            this[$mouse].leftBtn = 'down';
            this.update(this[$mouse]);
        }
    }

    // abstract method
    render(g) {
        g.clearRect(0, 0, g.canvas.width, g.canvas.height);
        g.canvas.style.cursor = this.pointer ? 'pointer' : undefined;
    }

    // abstract method
    update(_mouse) {
    }
}

module.exports = { View };
