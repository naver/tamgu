"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.activate = activate;
exports.deactivate = deactivate;
const vscode = require("vscode");
function activate(context) {
    let disposableCommand = vscode.commands.registerCommand('extension.runTamgu', () => {
        runCurrentTamguFile();
    });
    context.subscriptions.push(disposableCommand);
}
function runCurrentTamguFile() {
    const editor = vscode.window.activeTextEditor;
    if (editor) {
        const document = editor.document;
        const filePath = document.fileName;
        runTamguFile(filePath);
    }
    else {
        vscode.window.showErrorMessage('No active editor found.');
    }
}
function runTamguFile(filePath) {
    if (filePath.endsWith('.tmg')) {
        // Chercher un terminal existant nommé "Tamgu"
        let terminal = vscode.window.terminals.find(t => t.name === 'Tamgu');
        // Si le terminal "Tamgu" n'existe pas, créer un nouveau terminal
        if (!terminal) {
            terminal = vscode.window.createTerminal('Tamgu');
        }
        terminal.show();
        terminal.sendText(`tamgu "${filePath}"`);
    }
    else {
        vscode.window.showErrorMessage('The current file is not a Tamgu (.tmg) file.');
    }
}
function deactivate() { }
//# sourceMappingURL=extension.js.map