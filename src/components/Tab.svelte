<script>
  import { onMount, onDestroy } from 'svelte';
  import { render } from '../render';

  export let Neutralino;
  export let filename;
  let source = '';
  let renderedSource = '';
  let status = '';
  let statusClass = '';
  let renderTimeout = null;
  const renderInterval = 50; // ms

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

  function updateRenderedMarkdown() {
    clearTimeout(renderTimeout);

    // Make a new timeout set to go off in 'renderInterval' ms
    renderTimeout = setTimeout(() => {
      renderedSource = render(source);
    }, renderInterval);
  }

  function updateVScroll(evt) {
    const currentPane = evt.target.classList.contains('side') ? evt.target : evt.target.closest('.side');
    const scrollTop = evt.currentTarget.scrollTop;
    const scrollTopMax = evt.currentTarget.scrollHeight + evt.currentTarget.clientHeight;
    const paneLabel = currentPane.classList.contains('leftpane') ? 'rightpane' : 'leftpane textarea';

    /* Update Other Pane Vertical scroll */
    const pane = currentPane.closest('.container').querySelector(`.side.${paneLabel}`);
    const paneScrollTopMax = pane.scrollHeight + pane.clientHeight;
    const newScrollTop = (scrollTop / scrollTopMax) * paneScrollTopMax;

    /* Require delta to > 1.5 to avoid ping pong between the two panes
     * due to rounding errors
     */
    if (Math.abs(pane.scrollTop - newScrollTop) > 1.5) {
      pane.scrollTop = newScrollTop;
    }
  }

  onMount(() => {
    /* Read filename and populate tab */
    if (filename) {
      Neutralino.filesystem.readFile(filename, (result) => {
        source = result.content;
        renderedSource = render(source);
      }, () => {
        status = `ERROR: Could not load "${filename}"`;
        statusClass = 'alert';
      });
    }
  });
  onDestroy(() => {
    clearTimeout(renderTimeout);
  });
</script>

<style>
  /* Header */
  .header {
    width: 100%;
    min-height: 2em;
    height: 10%;
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
  .container {
    height: 85%;
    display: flex;
    align-items: center;
  }
  .side {
    width: 48%;
    margin:auto;
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
<div class="container">
  <!-- Left Side : source code -->
  <div class="side leftpane">
    <textarea bind:value={source} on:keyup={updateRenderedMarkdown}
    on:scroll={updateVScroll}></textarea>
  </div>

  <!-- Right Side: Rendered code -->
  <div class="side rightpane markdown-body" on:scroll={updateVScroll}>
    {@html renderedSource}
  </div>
</div>
