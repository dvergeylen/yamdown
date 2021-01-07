<script>
  import markdownIt from 'markdown-it';
  import katex from 'katex';

  export let Neutralino;
  export let filename;
  let source = '';
  let renderedSource = '';
  let status = '';
  let statusClass = '';

  const md = markdownIt({
    html: true,
  });

  function saveDocument() {
    status = '';
    statusClass = '';

    Neutralino.filesystem.writeFile(filename, source, () => {
      status = `File saved successfully`;
      statusClass = 'notice';
    }, (error) => {
      status = `ERROR: Could not save "${filename}": ${JSON.stringify(error)}`;
      statusClass = 'alert';
    });
  }

  /* Read filename and populate tab */
  if (filename) {
    Neutralino.filesystem.readFile(filename, (result) => {
      source = result.content;
    }, () => {
      status = `ERROR: Could not load "${filename}"`;
      statusClass = 'alert';
    });
  }

  /* This function applies rules to render a Markdown document:
   * - with Latex translation ($...$ and $$...$$ statements)
   * - with absolute image paths (TODO)
   */
  function render(src) {
    let result;

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
     * - Exclude '$ ' and or ' $' open close markups (followed / preceded by space like chars), 
     *   otherwise can lead to false positives with '$' the currency
     * - Exclude if followed by digit (means currency) '(?!\d)' is a negative lookahead
     * - Exclude if pattern is between code ranges (see above)
     */
    result = result.replace(/\${1,2}([^\s\$][^\$\n]+?[^\s\$])\${1,2}(?!\d)/g, (match, p1, offset) => {
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

  $: renderedSource = render(source);
</script>

<style>
  /* Header */
  .header {
    width: 100%;
    min-height: 2em;
    display: flex;
    align-items: center;
  }
  .header .leftpane {
    width: 30%;
  }
  .header .rightpane {
    width: 70%;
  }
  #status-bar {
    width: 100%;
    min-height: 1em;
    padding: 1em;
  }
  .notice {
    background: green;
  }
  .alert {
    background: red;
  }

  /* Main two panes */
  #container {
    height: 98%;
    display: flex;
    align-items: center;
  }
  .side {
    width: 50%;
    height: 100%;
    padding-left: 0.5em;
    padding-right: 0.5em;
  }
  .side.leftpane textarea {
    width: 100%;
    height: 100%;
  }
  .side.rightpane {
    overflow-y: scroll;
  }
</style>


<!-- Header -->
<div class="header">
  <div class="leftpane">
    <button on:click={saveDocument}>
      Save
    </button>
  </div>
  <div class="rightpane">
    <div id="status-bar" class="{statusClass}">
      {status}
    </div>
  </div>
</div>

<!-- Main two panes -->
<div id="container">
  <!-- Left Side : source code -->
  <div class="side leftpane">
    <textarea bind:value={source}></textarea>
  </div>

  <!-- Right Side: Rendered code -->
  <div class="side rightpane">
    {@html renderedSource}
  </div>
</div>
