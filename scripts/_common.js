/**
 * 
 * @author Matthew Evans
 * @module wtfsystems/wtengine-tools
 * @see README.md
 * @copyright MIT see LICENSE.md
 * 
 */

const fs = require('fs')
const shell = require('shelljs')
const inquirer = require('inquirer')

/**
 * Configuration settings
 */
const config = {
    checkApps: [ "cmake", "git" ],
    gitURLs: {
        "allegro": "https://github.com/liballeg/allegro5",
        "physfs": "https://github.com/icculus/physfs"
    }
}
exports.config = config

/**
 * Constants
 */
const constants = {
    CONFIG_SCRIPT:     `npm run wte-config`,
    SYSCHECK_SCRIPT:   `npm run wte-syscheck`,
    SETTINGS_LOCATION: `${__dirname}/../settings.json`,

    /* Font colors */
    RED:    `\x1b[31m`,
    GREEN:  `\x1b[32m`,
    YELLOW: `\x1b[33m`,
    BLUE:   `\x1b[34m`,
    CYAN:   `\x1b[36m`,
    DIM:    `\x1b[2m`,
    CLEAR:  `\x1b[0m`
}
exports.constants = constants

/**
 * Display an error message and exit script.
 * @param {String} message Message to display.
 */
const scriptError = (message) => {
    process.stdout.write(`${constants.RED}Error:  ${message}  Exiting...${constants.CLEAR}\n`)
    process.exit(1)
}
exports.scriptError = scriptError

/**
 * Confirmation prompt (wip)
 * @param {String} message Message to display.
 * @param {boolean} dvalue Default answer (Y - true | N - false)
 * @returns {boolean} True if default answer, else false
 */
const confirmPrompt = (message, dvalue) => {
    if(dvalue == undefined) dvalue = true
    return (async () => {
        const prompt = await inquirer.prompt([{
            default: dvalue,
            name: 'conf',
            type: 'confirm',
            message: `${constants.YELLOW}${message}`
        }]).then(res => { return res.conf })
        console.log(prompt)
        return prompt
    })()
    /*const prompt = inquirer.prompt([{
        default: dvalue,
        name: 'conf',
        type: 'confirm',
        message: `${constants.YELLOW}${message}`
    }])
    process.stdout.write(`${constants.CLEAR}\n`)
    const res = await prompt.then(res => { return res.conf })
    return res*/
}
exports.confirmPrompt = confirmPrompt

/**
 * Verify write access to engine settings file.
 * @param {String} permissions File permissions to check, 'rwx' format.
 * Passing nothing checks if the file simply exists.
 * @returns {boolean} True if permission checks succeded.
 * On fail, displays a script error and exit.
 */
const checkSettings = (permissions) => {
    let checkFlags = []
    if(permissions == undefined) checkFlags.push(fs.constants.F_OK)
    if(permissions.includes("r") || permissions.includes("R")) checkFlags.push(fs.constants.R_OK)
    if(permissions.includes("w") || permissions.includes("W")) checkFlags.push(fs.constants.W_OK)
    if(permissions.includes("x") || permissions.includes("X")) checkFlags.push(fs.constants.X_OK)

    if(checkFlags.length == 0) scriptError(`Unable to check settings file!  No proper tests requested!`)

    checkFlags.forEach(fFlag => {
        fs.access(constants.SETTINGS_LOCATION, fFlag, (err) => { scriptError(err) })
    })
    return true
}
exports.checkSettings = checkSettings

/**
 * Load engine settings.
 * @returns {JSON} Settings JSON object.  False on fail.
 */
const loadSettings = () => {
    try {
        const settings = fs.readFileSync(constants.SETTINGS_LOCATION)
        return JSON.parse(settings)
    } catch (err) {
        return false
    }
}
exports.loadSettings = loadSettings

/**
 * Save engine settings.
 * @param {JSON} settings Settings as JSON object.
 * @returns {boolean} True if settings saved.
 * On fail, display error and exit running script.
 */
const saveSettings = (settings) => {
    if(!(settings instanceof Object)) scriptError(`Settings format not valid.`)

    const oldSettings = loadSettings()
    if(oldSettings) settings = oldSettings.concat(settings)

    try {
        fs.writeFileSync(constants.SETTINGS_LOCATION, JSON.stringify(settings))
        process.stdout.write(`${constants.GREEN}Settings saved.${constants.CLEAR}\n`)
    } catch (err) {
        scriptError(err)
    }
    return true
}
exports.saveSettings = saveSettings

/**
 * Check for necessary applications.
 * @returns {boolean} True if all apps are found.
 * On not found, displays a script error and exit.
 */
const checkApps = () => {
    process.stdout.write(`Checking for necessary applications...\n`)
    config.checkApps.forEach((appCheck) => {
        if(shell.which(appCheck)) process.stdout.write(`${constants.GREEN}  > '${appCheck}' found.${constants.CLEAR}\n`)
        else scriptError(`'${appCheck}' not found.`)
    })
    process.stdout.write(`${constants.CLEAR}\n`)
    return true
}
exports.checkApps = checkApps

/**
 * Run the system check script.
 * @returns {boolean} True if the script was successful, else false.
 */
const runSysCheckScript = () => {
    let res = shell.exec(constants.SYSCHECK_SCRIPT, { async: false })
    if(res.code > 0) return false
    return true
}
exports.runSysCheckScript = runSysCheckScript

/**
 * Run the configuration script.
 * @returns {boolean} True if the script was successful, else false.
 */
const runConfigScript = () => {
    let res = shell.exec(constants.CONFIG_SCRIPT, { async: false })
    if(res.code > 0) return false
    return true
}
exports.runConfigScript = runConfigScript
