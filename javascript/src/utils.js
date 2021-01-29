/* Converts a function of signature fn(...args, cbSuccess, cbError)
 * into a promise function.
 * Returns a *function*.
 */
function promisify(fn) {
  return (...args) => {
    return new Promise((resolve, reject) => fn(...args, (results) => {
      resolve(results);
    }, reject));
  }
}

// PDF Export:
// import puppeteer from 'puppeteer-core';
// async function exportToPDF(filename) {
//   const browser = await puppeteer.launch({
//     executablePath: '/usr/bin/chromium',
//   });
//   const page = await browser.newPage();

//   await page.goto(`http://localhost:${NL_PORT}`, {
//     waitUntil: 'networkidle2',
//   });
//   await page.pdf({
//     path: filename,
//     format: 'A4',
//     printBackground: true,
//   });

//   await browser.close();
// }

export {
  promisify,
};