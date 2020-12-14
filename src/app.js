import App from './App.svelte';

Neutralino.init({
    load: function() {

        const app = new App({
            target: document.body,
            props: {}
        });
        
        window.app = app;
    },
    pingSuccessCallback : function() {
        
    },
    pingFailCallback : function() {
            app.exit();
    }
});