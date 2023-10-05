import Vue from 'vue';
import router from './router/index';
import moment from 'moment';

Vue.prototype.moment = moment;

new Vue({
  el: '#app',
  router,
  // render: h => h(App)
});
