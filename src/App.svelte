<script>
  import path from 'path';
  import Tab from './components/Tab.svelte';

  export let Neutralino;
  export let NL_ARGS;

  $: filenames = NL_ARGS.map((arg) => path.basename(arg));
</script>

<style>
  /* Tabs */
  .tabs {
    display: flex;
    align-items: flex-end;
    border-bottom: 1px solid gray;
    height: 5%;
  }
  .tabs .tab {
    border-left: 1px solid gray;
    border-right: 1px solid gray;
    border-top: 1px solid gray;
    border-bottom: 1px solid white;
    border-top-left-radius: 0.2em;
    border-top-right-radius: 0.2em;
    padding: 0.5em;
    color: #313131;
    font-size: 0.8em;
  }
  .tabs .tab:first-child {
    margin-left: 0.5em;
  }
  .tabs .tab:lat-child {
    margin-right: 0.5em;
  }
</style>

<!-- Tabs List -->
<div class="tabs">
  {#each filenames as filename, i}
    {#if i > 0 }
      <div class="tab">
        {filename}
      </div>
    {/if}
  {/each}
</div>

{#each NL_ARGS as filename, i}
  {#if i > 0 }
    <Tab {filename} {Neutralino}/>
  {/if}
{/each}
