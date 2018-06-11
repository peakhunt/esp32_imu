import Vue from 'vue'
import axios from 'axios'
import Vuetify from 'vuetify'
import 'vuetify/dist/vuetify.css'

import App from './App'
import router from './router'
import store from './store'

import VDragged from 'v-dragged'
import vuewheel from 'vuewheel'

import 'material-design-icons/iconfont/material-icons.css'

Vue.use(Vuetify)
Vue.use(VDragged)
Vue.use(vuewheel)

if (!process.env.IS_WEB) Vue.use(require('vue-electron'))
Vue.http = Vue.prototype.$http = axios
Vue.config.productionTip = false

/* eslint-disable no-new */
new Vue({
  components: { App },
  router,
  store,
  template: '<App/>'
}).$mount('#app')
