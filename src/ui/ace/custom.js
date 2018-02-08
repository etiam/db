var mywidth = 3;
editor.session.gutterRenderer = {
    setWidth: function(width) {
        mywidth = width;
    },
    getWidth: function(session, lastLineNumber, config) {
        return mywidth * config.characterWidth;
    },
    getText: function(session, row) {
        return row+1;
    }
};

