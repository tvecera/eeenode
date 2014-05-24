#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
//#include <string.h>

#include "ec.h"
#include "options.h"

struct proc_dir_entry *proc_entry;

struct eeefsb_proc_file
{
    char *name;
    struct file_operations *fops;
};

char msg[4];

static struct proc_dir_entry *eeefsb_proc_rootdir;
#define EEEFSB_PROC_WRITEFUNC(NAME) \
    ssize_t eeefsb_proc_write_##NAME (struct file *f, const char __user *buf, size_t len, loff_t *off)
#define EEEFSB_PROC_SHOWFUNC(NAME) \
    static int eeefsb_proc_show_##NAME (struct seq_file *m, void *v)
#define EEEFSB_PROC_OPENFUNC(NAME) \
    static int eeefsb_proc_open_##NAME (struct inode *inode, struct file *file) \
      { \
        return single_open(file, eeefsb_proc_show_##NAME, NULL); \
      }
#define EEEFSB_PROC_FILES_BEGIN \
    static struct eeefsb_proc_file eeefsb_proc_files[] = {
#define EEEFSB_PROC_RW(NAME) \
    { #NAME, \
      (struct file_operations *) { \
        (struct file_operations []) { \
          { .owner=THIS_MODULE, .open=eeefsb_proc_open_##NAME, .read=seq_read, .write=eeefsb_proc_write_##NAME, .llseek = seq_lseek, .release = single_release, } \
        } \
      } \
    }
#define EEEFSB_PROC_RO(NAME) \
    { #NAME, \
      (struct file_operations *) { \
        (struct file_operations []) { \
          { .owner=THIS_MODULE, .open=eeefsb_proc_open_##NAME, .read=seq_read, .llseek = seq_lseek, .release = single_release, } \
        } \
      } \
    }
#define EEEFSB_PROC_FILES_END \
    { NULL, NULL } };


/************************************
* VOLTAGE
*************************************/
EEEFSB_PROC_SHOWFUNC(voltage)
{
  int voltage = 0;
  voltage = (int)eeefsb_get_voltage();
  seq_printf(m, "%d\n", voltage);
  return 0;
}

EEEFSB_PROC_WRITEFUNC(voltage)
{
  copy_from_user(msg,buf,len-1);
  
  if (strcmp(msg, EEE_FSB_HIGH) == 0) 
  {
    printk(KERN_INFO "EEE FSB=high");
    eeefsb_set_voltage(1);
  } 
  else if (strcmp(msg, EEE_FSB_LOW) == 0) 
  {
    printk(KERN_INFO "EEE FSB=low");
    eeefsb_set_voltage(0);
  } 
  else 
  {
    printk(KERN_ERR "EEE FSB=[%s] - undefined variable\n", msg);
  }
  return len;
}

EEEFSB_PROC_OPENFUNC(voltage)

EEEFSB_PROC_SHOWFUNC(temperature)
{
  unsigned int t = eeefsb_get_temperature();
  seq_printf(m, "%d\n", t);
  return 0;
}

EEEFSB_PROC_OPENFUNC(temperature)

/************************************
* FAN CONTROL
*************************************/
EEEFSB_PROC_SHOWFUNC(fan_control)
{
  seq_printf(m, "%d\n", eeefsb_fan_get_manual());
  return 0;
}

EEEFSB_PROC_WRITEFUNC(fan_control)
{
  copy_from_user(msg,buf,len-1);
  
  if (strcmp(msg, EEE_FAN_CONTROL_AUTO) == 0) 
  {
    printk(KERN_INFO "EEE FAN CONTROLL=AUTO");
    eeefsb_fan_set_control(1);
  } 
  else if (strcmp(msg, EEE_FAN_CONTROL_MANUAL) == 0) 
  {
    printk(KERN_INFO "EEE FAN CONTROL=MANUAL");
    eeefsb_fan_set_control(0);
  } 
  else 
  {
    printk(KERN_ERR "EEE FAN CONTROL=[%s] - undefined variable\n", msg);
  }
  return len;
}

EEEFSB_PROC_OPENFUNC(fan_control)

/************************************
* FAN RPM
*************************************/
EEEFSB_PROC_SHOWFUNC(fan_rpm)
{
  int rpm = eeefsb_fan_get_rpm();
  seq_printf(m, "%d\n", rpm);
  return 0;
}

EEEFSB_PROC_OPENFUNC(fan_rpm)

/************************************
* FAN SPEED
*************************************/
EEEFSB_PROC_SHOWFUNC(fan_speed)
{
  int speed = eeefsb_fan_get_speed();
  seq_printf(m, "%d\n", speed);
  return 0;
}

EEEFSB_PROC_OPENFUNC(fan_speed)

EEEFSB_PROC_FILES_BEGIN
    EEEFSB_PROC_RW(voltage),
    EEEFSB_PROC_RO(temperature),
    EEEFSB_PROC_RW(fan_control),
    EEEFSB_PROC_RO(fan_speed),
    EEEFSB_PROC_RO(fan_rpm),
EEEFSB_PROC_FILES_END

static int __init hello_proc_init(void) {
  int i;

  /* Create the /proc/eeefsb directory. */  
  eeefsb_proc_rootdir = proc_mkdir("eeefsb", NULL);
  if (!eeefsb_proc_rootdir)
  {
    printk(KERN_ERR "eeefsb: Unable to create /proc/eeefsb\n");
    return false;
  }

  for (i=0; eeefsb_proc_files[i].name; i++)
  {
    struct eeefsb_proc_file *f = &eeefsb_proc_files[i];
    proc_entry = proc_create(f->name, 0, eeefsb_proc_rootdir, f->fops);

    if (proc_entry == NULL) {
      printk(KERN_ERR "EEE FSB: Couldn't create proc entry\n");
      return -ENOMEM;
    }
  }
  
  printk(KERN_INFO "EEE FSB: Module loaded successfully\n");
  //msg=kmalloc(GFP_KERNEL,10*sizeof(char));
  return 0;
}

static void __exit hello_proc_exit(void) 
{
  int i;
  for (i = 0; eeefsb_proc_files[i].name; i++)
    {
      remove_proc_entry(eeefsb_proc_files[i].name, eeefsb_proc_rootdir);
    }
  remove_proc_entry("eeefsb", NULL);
}

module_init(hello_proc_init);
module_exit(hello_proc_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Georg Kaindl <gkaindl --AT-- mac.com>");
MODULE_DESCRIPTION("kernel driver for the rfm12b digital radio module");
MODULE_VERSION("0.0.1");
