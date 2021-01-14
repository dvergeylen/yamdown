<script>
  import { promisify } from './utils';
  import Tab from './components/Tab.svelte';

  export let Neutralino;
  export let NL_ARGS;

  const getEnvVar = promisify(Neutralino.os.getEnvar);
  const promisePWD = getEnvVar('PWD');
</script>


<!-- Tabs List -->
<div class="header">
  <!-- TODO: Tabs headers should come here -->
  {#await promisePWD}
    <p>...waiting</p>
  {:then pwd}
    <p>Current cwd is {JSON.stringify(pwd)}</p>
  {:catch error}
    <p style="color: red">{error.message}</p>
  {/await}
</div>

{#each NL_ARGS as filename, i}
  {#if i > 0 }
    <Tab {filename} {Neutralino}/>
  {/if}
{/each}
