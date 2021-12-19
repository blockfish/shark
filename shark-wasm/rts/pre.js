Module = {};

let RTS = {
    _textDec: new TextDecoder('utf8'),
    _textEnc: new TextEncoder('utf8'),
    _recvBuf: [],
    _parked: false,

    log(lvl, sys, msg) {
        let getStr = (ptr) => {
            let heap = Module['HEAPU8'];
            let end = ptr;
            while (heap[end] != 0) end++;
            return this._textDec.decode(heap.subarray(ptr, end));
        };
        let prefix = ['ERR', 'WARN', 'INFO', 'DEBUG', 'TRACE'][lvl - 1];
        let method = ['error', 'warn', 'log', 'log', 'log'][lvl - 1];
        console[method](`${prefix} [bf::${getStr(sys)}] ${getStr(msg)}`);
    },

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
            self['close']();
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
