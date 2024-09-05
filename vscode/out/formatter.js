const vscode = require('vscode');
const { exec } = require('child_process');
function formatDocument(document) {
    const text = document.getText();
    return new Promise((resolve, reject) => {
        exec(`dotnet format --include ${document.uri.fsPath}`, (error, stdout, stderr) => {
            if (error) {
                reject(error);
            }
            else {
                resolve([vscode.TextEdit.replace(new vscode.Range(0, 0, document.lineCount, 0), stdout)]);
            }
        });
    });
}
function activate(context) {
    context.subscriptions.push(vscode.languages.registerDocumentFormattingEditProvider('votre-langage', {
        provideDocumentFormattingEdits: formatDocument
    }));
}
exports.activate = activate;
//# sourceMappingURL=formatter.js.map