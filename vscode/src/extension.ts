import * as vscode from 'vscode';

export function activate(context: vscode.ExtensionContext) {
    let disposable = vscode.commands.registerCommand('extension.runTamgu', () => {
        const editor = vscode.window.activeTextEditor;

        if (editor) {
            const document = editor.document;
            const filePath = document.fileName;

            if (filePath.endsWith('.tmg')) {
                const terminal = vscode.window.createTerminal('Tamgu');
                terminal.show();
                terminal.sendText(`tamgu "${filePath}"`);
            } else {
                vscode.window.showErrorMessage('The current file is not a Tamgu (.tmg) file.');
            }
        } else {
            vscode.window.showErrorMessage('No active editor found.');
        }
    });

    context.subscriptions.push(disposable);
}

export function deactivate() {}
