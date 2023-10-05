import Vue from 'vue'
import Router from 'vue-router'
import Index from '../component/index.vue'
import Login from '../component/login.vue'
import Home from '../component/home.vue'
import Info from '../component/agentInfo.vue'
import Password from '../component/updatePass.vue'
import Below from '../component/below.vue'
import AddProxy from '../component/addAgent.vue'
import Record from '../component/record.vue'
import Send from '../component/present.vue'
import Award from '../component/award.vue'
import awardRecord from '../component/awardRecord.vue'
import expendInfo from '../component/expendInfo.vue'
import setPassword from '../component/setPassword.vue'
import grantRecord from '../component/grantRecord.vue'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/Login',
      component: Login
    },
    {
      path: '/Index',
      component: Index,
      props: route => ({ token: route.query.token })
    },
    {
      path: '/Home',
      component: Home,
      props: route => ({ nav: 'Home' })
    },
    {
      path: '/Info',
      component: Info
    },
    {
      path: '/AddProxy',
      component: AddProxy,
      props: route => ({ nav: 'Below' })
    },
    {
      path: '/Below',
      component: Below,
      props: route => ({ type: '', nav: 'Below' })
    },
    {
      path: '/BelowAgent',
      component: Below,
      props: route => ({ type: 'agent', nav: 'Below' })
    },
    {
      path: '/BelowUser',
      component: Below,
      props: route => ({ type: 'user', nav: 'Below' })
    },
    {
      path: '/Award',
      component: Award,
      props: nav => ({ nav: 'Award' })
    },
    {
      path: '/payInfo',
      component: awardRecord,
      props: nav => ({ nav: 'Award',type:'pay' })

    },
    {
      path: '/expendInfo',
      component: awardRecord,
      props: nav => ({ nav: 'Award',type:'revenue'})
    },
    {
      path: '/grantDiamondInfo',
      component: grantRecord,
      props: nav => ({ nav: 'Award',type:'diamond' })
    },
    {
      path: '/grantGoldInfo',
      component: grantRecord,
      props: nav => ({ nav: 'Award',type:'gold' })
    },
    {
        path: '/setPassword',
        component: setPassword
      },
    {
      path: '/',
      redirect: '/Index'
    }
  ]
})
