 /*
 * This file is part of the codeview Editor
 * This program is free software licensed under the GNU LGPL. You can
 * find a copy of this license in LICENSE in the top directory of
 * the source code.
 *
 * Copyright 2013    Illya Kovalevskyy   <illya.kovalevskyy@gmail.com>
 *
 */

function property(param, value) {
    if (arguments.length == 1) {
        // Get
        return sessionStorage[param];
    } else {
        // Set
        sessionStorage[param] = value;
    }
}

//var sprintf = require('sprintf.jsxass').sprintf;

property("lines", 1);
property("text", "");

var timerid = -1;

// All necessary calls here
function handleEvents() {
    var newLines = editor.session.getLength();
    if (newLines != property("lines")) {
        property("lines", newLines);
        codeview.onLinesChanged(newLines);
    }

//    var newText = editor.getValue();
//    if (newText != property("text")) {
//        property("text", newText);
//        codeview.onTextChanged();
//    }

    clearTimeout(timerid);
    timerid = -1;
}

editor.on('mousemove', function(e) {
    gw = editor.session.gutterRenderer.getWidth(0, 0, editor.renderer.layerConfig);
    
    cx = e.clientX / editor.renderer.layerConfig.characterWidth;
    cy = e.clientY / editor.renderer.layerConfig.lineHeight;
    
    //str = sprintf('%d %d %d %d', e.clientX, e.clientY, cx, cy);
    //console.log(str);
    
    console.log(e.view);
    
//    console.log(editor.renderer.layerConfig.characterWidth);
//    console.log(editor.renderer.layerConfig.lineHeight);
//    codeview.onMouseMove(e.clientX / editor.renderer.layerConfig.characterWidth, 
//                         e.clientY / editor.renderer.layerConfig.lineHeight);

    codeview.onMouseMove(e.clientX, e.clientY);
});


// On each change with the editor
// Some events haven't been finished yet
// Thats why we schedule it
editor.on('change', function() {
    if (timerid > 0) {
        clearTimeout(timerid);
    }

    timerid = setTimeout(handleEvents, 50);
});


// gutter width
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
