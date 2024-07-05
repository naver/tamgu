import * as vscode from 'vscode';

export function activate(context: vscode.ExtensionContext) {
    let disposableCommand = vscode.commands.registerCommand('extension.runTamgu', () => {
        runCurrentTamguFile();
    });

    context.subscriptions.push(disposableCommand);

    let disposableOpen = vscode.workspace.onDidOpenTextDocument((document) => {
        if (document.fileName.endsWith('.tmg')) {
            runTamguFile(document.fileName);
        }
    });

    let disposableSave = vscode.workspace.onDidSaveTextDocument((document) => {
        if (document.fileName.endsWith('.tmg')) {
            runTamguFile(document.fileName);
        }
    });

    context.subscriptions.push(disposableOpen);
    context.subscriptions.push(disposableSave);
}

function runCurrentTamguFile() {
    const editor = vscode.window.activeTextEditor;

    if (editor) {
        const document = editor.document;
        const filePath = document.fileName;

        runTamguFile(filePath);
    } else {
        vscode.window.showErrorMessage('No active editor found.');
    }
}

function runTamguFile(filePath: string) {
    if (filePath.endsWith('.tmg')) {
        // Chercher un terminal existant nommé "Tamgu"
        let terminal = vscode.window.terminals.find(t => t.name === 'Tamgu');
        
        // Si le terminal "Tamgu" n'existe pas, créer un nouveau terminal
        if (!terminal) {
            terminal = vscode.window.createTerminal('Tamgu');
        }

        terminal.show();
        terminal.sendText(`tamgu "${filePath}"`);
    } else {
        vscode.window.showErrorMessage('The current file is not a Tamgu (.tmg) file.');
    }
}

export function deactivate() {}
