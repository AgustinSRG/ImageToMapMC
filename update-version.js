// Script to update the version number
// Usage: node update-version.js A.B.C

"use strict";

const FS = require("fs");
const Path = require("path");

function updateFile(file, callback) {
    const contents = FS.readFileSync(file).toString();
    const newContents = callback(contents);
    FS.writeFileSync(file, newContents);
    console.log("UPDATED: " + file);
}

function main() {
    const arg = process.argv[2] || "";

    if (!arg) {
        console.log("Usage: node update-version.js A.B.C");
        return 1;
    }

    const parts = arg.split(".");

    if (parts.length !== 3) {
        console.log("Usage: node update-version.js A.B.C");
        return 1;
    }

    const MAJOR = parseInt(parts[0], 10);
    const MINOR = parseInt(parts[1], 10);
    const REVISION = parseInt(parts[2], 10);

    if (isNaN(MAJOR) || isNaN(MINOR) || isNaN(REVISION) || MAJOR < 0 || MINOR < 0 || REVISION < 0) {
        console.log("Usage: node update-version.js A.B.C");
        return 1;
    }

    const VERSION = MAJOR + "." + MINOR + "." + REVISION;
    const DATE = (new Date()).toISOString().split("T")[0];

    console.log(`Changing version to ${VERSION} | Date: ${DATE}`);

    updateFile(Path.resolve(__dirname, "build.bat"), contents => {
        return contents
            .replace(/ImageToMapMC\-[0-9]+\.[0-9]+\.[0-9]+\-Windows\-x64\.zip/g, `ImageToMapMC-${VERSION}-Windows-x64.zip`);
    });

    updateFile(Path.resolve(__dirname, "build.sh"), contents => {
        return contents
            .replace(/ImageToMapMC\-[0-9]+\.[0-9]+\.[0-9]+\-linux\-x64\.tar\.gz/g, `ImageToMapMC-${VERSION}-linux-x64.tar.gz`);
    });

    updateFile(Path.resolve(__dirname, "wix", "make-wix-installer.bat"), contents => {
        return contents
            .replace(/ImageToMapMC\-[0-9]+\.[0-9]+\.[0-9]+\-x64\.msi/g, `ImageToMapMC-${VERSION}-x64.msi`);
    });

    updateFile(Path.resolve(__dirname, "wix", "Product.wxs"), contents => {
        return contents
            .replace(/Name=\"ImageToMapMC\" Version=\"[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+\"/, `Name="ImageToMapMC" Version="${VERSION}.0"`);
    });

    updateFile(Path.resolve(__dirname, "src", "version.h"), contents => {
        return contents
            .replace(/APP_VERSION\s\"[0-9]+\.[0-9]+\.[0-9]+\"/, `APP_VERSION "${VERSION}"`);
    });

    console.log("DONE!");
}

process.exit(main() || 0);