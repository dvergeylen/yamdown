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


export {
  promisify,
};