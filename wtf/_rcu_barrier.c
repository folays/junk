static void _rcu_barrier(struct rcu_state *rsp,
                         void (*call_rcu_func)(struct rcu_head *head,
                                               void (*func)(struct rcu_head *head)))
{
  BUG_ON(in_interrupt());
  /* Take mutex to serialize concurrent rcu_barrier() requests. */
  mutex_lock(&rcu_barrier_mutex);
  init_completion(&rcu_barrier_completion);
  /*
   * Initialize rcu_barrier_cpu_count to 1, then invoke
   * rcu_barrier_func() on each CPU, so that each CPU also has
   * incremented rcu_barrier_cpu_count.  Only then is it safe to
   * decrement rcu_barrier_cpu_count -- otherwise the first CPU
   * might complete its grace period before all of the other CPUs
   * did their increment, causing this function to return too
   * early.
   */

  atomic_set(&rcu_barrier_cpu_count, 1);
  preempt_disable(); /* stop CPU_DYING from filling orphan_cbs_list */
  rcu_adopt_orphan_cbs(rsp);
  on_each_cpu(rcu_barrier_func, (void *)call_rcu_func, 1);
  
  printk(KERN_INFO " before preempt_enabled() : %d", atomic_read(&rcu_barrier_cpu_count));

  preempt_enable(); /* CPU_DYING can again fill orphan_cbs_list */
  if (atomic_dec_and_test(&rcu_barrier_cpu_count))
    {
      printk(KERN_INFO "after preempt_enabled() : rcu_barrier_cpu_count is 0");
      complete(&rcu_barrier_completion);
    }
  else
    {
      printk(KERN_INFO "after preempt_enabled() : rcu_barrier_cpu_count is != 0");
    }

  while (1)
    {
      printk(KERN_INFO "wait ... (currently %d\n", atomic_read((&rcu_barrier_cpu_count)));
      if (wait_for_completion_timeout(&rcu_barrier_completion, 1))
        {
          prinkt(KER_INFO "completion done");
          break;
        }
    }
  mutex_unlock(&rcu_barrier_mutex);
}