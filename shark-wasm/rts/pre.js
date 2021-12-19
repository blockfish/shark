Module = {};

let RTS = {
    _textDec: new TextDecoder('utf8'),
    _textEnc: new TextEncoder('utf8'),
    _recvBuf: [],
    _parked: false,

    send(ptr, len) {
        let inp = Module['HEAPU8'].subarray(ptr, ptr + len);
        try {
            self['postMessage'](JSON.parse(this._textDec.decode(inp)));
            return len;
        } catch (e) {
            console.error('RTS.postMessage:', e);
            return -1;
        }
    },

    recv(ptr) {
        if (this._recvBuf.length === 0) {
            return 0;
        }
        if (ptr === 0) {
            return this._recvBuf[0].length;
        }
        let msg = this._recvBuf.shift();
        let out = Module['HEAPU8'].subarray(ptr, ptr + msg.length);
        out.set(msg);
        return msg.length;
    },

    _onMessage(msg) {
        let bytes = this._textEnc.encode(JSON.stringify(msg));
        this._recvBuf.push(bytes);
        this._unpark();
    },

    _poll() {
        switch (Module['_WW_poll']()) {
        case 0: // yield
            setTimeout(() => this._poll(), 0);
            break;

        case 1: // park
            this._parked = true;
            break;

        case 2: // shutdown
            self['close']();
            break;

        default: // error
            throw new Error('wasm event loop error');
        }
    },

    _unpark() {
        if (this._parked) {
            this._parked = false;
            this._poll();
        }
    }
};

self["onmessage"] = ev => RTS._onMessage(ev.data);
Module["postRun"] = [() => RTS._poll()];
