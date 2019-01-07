
'use strict';

const myx = require("../build/Release/my_extension");
console.log(myx);
//console.log(myx.hello());

const electron = require("electron");
//console.log(electron);

// アプリケーションをコントロールするモジュール
const app = electron.app;
app.disableHardwareAcceleration();
console.log(app.getGPUInfo("basic"));

// ウィンドウを作成するモジュール
const BrowserWindow = electron.BrowserWindow;

// メインウィンドウはGCされないようにグローバル宣言
let mainWindow;
let subWindow;

// 全てのウィンドウが閉じたら終了
app.on('window-all-closed', function() {
    if (process.platform != 'darwin') {
	app.quit();
    }
});


const {ipcMain} = require('electron');

ipcMain.on('asynchronous-message', (event, arg) => {
    console.log(arg);
    
    let buf = mainWindow.getNativeWindowHandle();
    console.log("mainWindow", buf);

    if (arg=="load") {
	myx.createNatWin(buf, buf.length);
    }
    else {
	console.log(myx.hello(buf, buf.length));
    }
    
    event.sender.send('asynchronous-reply', 'pong');
});

// Electronの初期化完了後に実行
app.on('ready', function() {
    // メイン画面の表示。ウィンドウの幅、高さを指定できる
    mainWindow = new BrowserWindow();
    // mainWindow.webContents.openDevTools({ detach: true })
    mainWindow.loadURL('file://' + __dirname + '/index.html');

    mainWindow.on('ready-to-show', function () {
        mainWindow.show();
        mainWindow.focus();

	let buf = mainWindow.getNativeWindowHandle();
	console.log("mainWindow", buf);
	console.log(myx.hello(buf, buf.length));
    });


    // ウィンドウが閉じられたらアプリも終了
    mainWindow.on('closed', function() {
	mainWindow = null;
    });

    let buf = mainWindow.getNativeWindowHandle();
    console.log("mainWindow", buf);
    console.log(myx.hello(buf, buf.length));

/*
    subWindow = new BrowserWindow();
    subWindow.loadURL('file://' + __dirname + '/index.html');

    // ウィンドウが閉じられたらアプリも終了
    subWindow.on('closed', function() {
	subWindow = null;
    });
    console.log("subWindow", subWindow.getNativeWindowHandle());
*/

});
