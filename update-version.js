// This script updates the version for
// the build scripts

// Run with NodeJS

const Path = require("path");
const FS = require("fs");

function main () {
    const version = process.argv[2];

    if (!version) {
        console.error("Usage: node update-version.js A.B.C");
        return;
    }

    const templates = FS.readdirSync(Path.resolve(__dirname, "script-templates"));

    for (let templateFile of templates) {
        const contents = FS.readFileSync(Path.resolve(__dirname, "script-templates", templateFile)).toString();
        FS.writeFileSync(Path.resolve(__dirname, templateFile), contents.replace(/\{VERSION\}/g, version));
        console.log("UPDATED: " + templateFile);
    }
}

main();
