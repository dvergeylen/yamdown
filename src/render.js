
import markdownIt from 'markdown-it';
import katex from 'katex';
import hljs from 'highlight.js/lib/core';
import javascript from 'highlight.js/lib/languages/javascript';
import diff from 'highlight.js/lib/languages/diff';
import json from 'highlight.js/lib/languages/json';
import ruby from 'highlight.js/lib/languages/ruby';
import bash from 'highlight.js/lib/languages/bash';

/* register common languages */
hljs.registerLanguage('javascript', javascript);
hljs.registerLanguage('diff', diff);
hljs.registerLanguage('json', json);
hljs.registerLanguage('ruby', ruby);
hljs.registerLanguage('bash', bash);

const md = markdownIt({
  html: true,
  highlight: (str, lang) => {
    if (lang && hljs.getLanguage(lang)) {
      try {
        return hljs.highlight(lang, str).value;
      } catch (__) { }
    }
    return ''; // use external default escaping
  },
});

/* This function applies rules to render a Markdown document:
  * - with Latex translation ($...$ and $$...$$ statements)
  * - with absolute image paths (TODO)
  */
function render(src) {
  let result;

  if (!src)
    return '';

  /* Markdown conversion */
  result = md.render(src);

  /* <code></code> markup detections
   * will return an array of str indexes containing <code> markup bounds
   * No further rules should be applied (e.g: latex conversion) within these ranges
   */
  const codesIndexes = Array.from(result.matchAll(/\<code.*?\>.*?\<\/code\>/gs)).map((elem) => {
    return [elem.index, elem.index + elem[0].length - 1]
  });

  /* Latex replacement: find $...$ or $$...$$ statements
   * - Exclude patterns with '$' in between otherwise consecutive patterns
   *   will be seen as one ($first$ and $second$ → would take very first and very last '$' of the two)
   * - Exclude if followed by digit (means currency) ('(?!\d)' is a negative lookahead)
   * - Exclude if pattern is between code ranges (see above)
   */
  result = result.replace(/\${1,2}([^\$\n]+?)\${1,2}(?!\d)/g, (match, p1, offset) => {
    const isInCodeRange = codesIndexes.reduce((acc, range) => range[0] <= offset && range[1] >= offset ? true : acc, false);
    if (isInCodeRange) {
      return match;
    } else {
      return katex.renderToString(p1, {
        throwOnError: false,
        displayMode: match.startsWith('$$'), /* '$$' pattern means separated paragraph, not inline */
      });
    }
  });

  return result;
}

export {
  render,
}