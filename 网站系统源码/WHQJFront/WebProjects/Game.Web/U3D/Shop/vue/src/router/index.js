import Vue from 'vue'
import Router from 'vue-router'
import Index from '@/components/Index'
import Order from '@/components/Order'
import Record from '@/components/Record'
import OverTime from '@/components/OverTime'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/',
      name: 'Index',
      component: Index
    },
    {
      path: '/order',
      name: 'Order',
      component: Order
    },
    {
      path: '/record',
      name: 'Record',
      component: Record
    },
    {
      path: '/overtime',
      name: 'OverTime',
      component: OverTime
    }
  ]
})
