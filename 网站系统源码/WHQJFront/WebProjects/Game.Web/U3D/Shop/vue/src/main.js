// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import Vue from 'vue'
import axios from 'axios'
import VueAxios from 'vue-axios'
import App from './App'
import router from './router'
import vDialogs from 'v-dialogs'
import layer from './layer/layer.js'



Vue.config.productionTip = false

/* eslint-disable no-new */
Vue.use(VueAxios, axios);
Vue.use(vDialogs);
Vue.use(layer);

new Vue({
  el: '#app',
  router,
  components: { App },
  template: '<App/>'
})
