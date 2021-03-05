<script>
  import { onMount, onDestroy } from 'svelte';
  import { render } from './render';

  const index = 1; // FIXME: no need of index anymore
  export let source = '';
  let renderedSource = '';
  let status = '';
  let statusClass = '';
  let renderTimeout = null;
  let latestRenderTimes = [];
  let renderInterval = 0; // ms

  function updateRenderedMarkdown() {
    clearTimeout(renderTimeout);

    // Make a new timeout set to go off in 'renderInterval' ms
    renderTimeout = setTimeout(() => {
      const start = Date.now();
      renderedSource = render(source);
      const end = Date.now();

      latestRenderTimes = [end - start, ...latestRenderTimes.slice(0, 9)];
    }, renderInterval);
  }

  function updateVScroll(evt) {
    const currentPane = evt.currentTarget.classList.contains('side') ?
      evt.currentTarget : evt.currentTarget.closest('.side');
    const scrollTop = evt.currentTarget.scrollTop;
    const scrollTopMax = evt.currentTarget.scrollHeight - evt.currentTarget.clientHeight;
    const paneLabel = currentPane.classList.contains('leftpane') ? 'rightpane' : 'leftpane textarea';

    /* Update Other Pane Vertical scroll */
    const pane = currentPane.closest('.container').querySelector(`.side.${paneLabel}`);
    const paneScrollTopMax = pane.scrollHeight - pane.clientHeight;
    const newScrollTop = (scrollTop / scrollTopMax) * paneScrollTopMax;

    /* Require delta to > 1.5 to avoid ping pong between the two panes
     * due to rounding errors
     */
    if (Math.abs(pane.scrollTop - newScrollTop) > 1.5) {
      pane.scrollTop = newScrollTop;
    }
  }

  // Dynamically compute optimal time between render intervals:
  function computeOptimizedRenderInterval(latestRenderTimes) {
    const n = Math.max(latestRenderTimes.length, 1); // to avoid dividing by 0
    const mean = latestRenderTimes.reduce((sum, e) => sum + e, 0) / n;
    const stdDev = Math.sqrt((1 / n) * latestRenderTimes.reduce((sum, e) => sum + Math.pow((e - mean), 2), 0))

    return Math.ceil(mean + 1.10 * stdDev); // Dynamically adapt Render intervals
  }

  onMount(() => {
    /* Read filename and populate tab */
    if (source) {
      renderedSource = render(source);
    }
  });
  onDestroy(() => {
    clearTimeout(renderTimeout);
  });


  $: renderInterval = computeOptimizedRenderInterval(latestRenderTimes);
</script>

<style>
  /* Header */
  .header {
    width: 100%;
    height: 5%;
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
    height: 88%;
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
    height: 99%;
  }
  .side.rightpane {
    overflow-y: scroll;
  }
</style>


<!-- Header -->
<div class="header">
  <div id="status-bar" class="{statusClass}">
    {status}
  </div>
</div>

<!-- Main two panes -->
<div id="pane-{index}" class="container">
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
