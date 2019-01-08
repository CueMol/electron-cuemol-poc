
const {ipcRenderer} = require('electron');
//const myx = require('electron').remote.require("../build/Release/my_extension");

console.log("Initializing...");
//console.log(myx);

window.addEventListener("load", function () {
    console.log("onLoad() called!!");

    elem = document.getElementById("mybutton");
    elem.addEventListener("click", function (e) {
	console.log("XXX.click() called!!");
	console.log(e.target);

	ipcRenderer.send('asynchronous-message', 'click');
    },
			  false);

    ipcRenderer.send('asynchronous-message', 'load');

    ipcRenderer.on('asynchronous-reply', function (event, arg) {
	console.log(arg);
    });
    
},
			false);


window.addEventListener("resize", function (e) {
    elem_ph = document.getElementById("placeholder");
    var rect = elem_ph.getBoundingClientRect();
/*    console.log("resize:", rect);
*/
    //myx.setWinPos(rect.x,rect.y,rect.width,rect.height);
    ipcRenderer.send('resize', rect.x,rect.y,rect.width,rect.height);

},
			false);


