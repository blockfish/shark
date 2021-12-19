LibraryManager.library['RTS_log'] = function(lvl, sys, msg) { return RTS.log(lvl, sys, msg); };
LibraryManager.library['RTS_send'] = function(ptr, len) { return RTS.send(ptr, len); };
LibraryManager.library['RTS_recv'] = function(ptr) { return RTS.recv(ptr); };

