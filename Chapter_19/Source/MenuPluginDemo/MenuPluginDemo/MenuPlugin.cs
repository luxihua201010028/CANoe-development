using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

using Vector.MenuPlugin;

namespace MenuPlugin
{
    public class MenuPlugin : IMenuPlugin
    {
        #region fields

        List<IMenuItem> mMenuItems = new List<IMenuItem>();
        string mText = "Plugin";

        #endregion

        public MenuPlugin()
        {
            //  添加几个菜单项
            mMenuItems.Add(new MenuItem("Item1"));
            mMenuItems.Add(new MenuItem("Item2"));
            MenuItem menuItem = new MenuItem("Item3");
            menuItem.AddMenuItem(new MenuItem("SubItem1"));
            menuItem.AddMenuItem(new MenuItem("SubItem2"));
            mMenuItems.Add(menuItem);
            mMenuItems.Add(new MenuItem("Open Notepad"));
        }

        private void UpdateItemStates()
        {
            // 演示如何激活或禁止菜单项
            // 或者切换选中和未选中的状态
            foreach (IMenuItem menuItem in mMenuItems)
            {
                MenuItem itemImpl = menuItem as MenuItem;
                if (itemImpl != null)
                {
                    if (itemImpl.Text == "Item1")
                    {
                        // 切换激活状态
                        itemImpl.Enabled = !itemImpl.Enabled;
                    }
                    else if (itemImpl.Text == "Item2")
                    {
                        // 切换选中状态
                        itemImpl.Checked = !itemImpl.Checked;
                    }
                }
            }
        }

        #region IMenuPlugin Members

        string IMenuPlugin.Text
        {
            get
            {
                return mText;
            }
        }

        IList<IMenuItem> IMenuPlugin.MenuItems
        {
            get
            {
                UpdateItemStates();
                return mMenuItems;
            }
        }

        void IMenuPlugin.ItemClicked(IMenuItem clickedItem)
        {
            MenuItem itemImpl = clickedItem as MenuItem;
            //声明一个程序信息类
            System.Diagnostics.ProcessStartInfo Info = new System.Diagnostics.ProcessStartInfo();

            //设置外部程序名
            Info.FileName = "notepad.exe";

            //设置外部程序的启动参数（命令行参数）为test.txt
            Info.Arguments = "test.txt";

            //设置外部程序工作目录为 C:\
            Info.WorkingDirectory = "C:\\";

            //声明一个程序类
            System.Diagnostics.Process Proc;

            if (itemImpl != null)
            {
                if (itemImpl.Text == "Open Notepad")
                {
                    try
                    {
                        //
                        //启动外部程序
                        //
                        Proc = System.Diagnostics.Process.Start(Info);
                    }
                    catch
                    {
                        //直接退出
                        return;
                    }


                    //等待3秒钟
                    Proc.WaitForExit(3000);

                    //如果这个外部程序没有结束运行则对其强行终止
                    if (Proc.HasExited == false)
                    {
                        //强行终止进程
                        Proc.Kill();
                    }

                }
                else
                {
                    MessageBox.Show("Item clicked: " + itemImpl.Text);
                }
            }
        }

        #endregion
    }

    public class MenuItem : IMenuItem
    {
        #region fields

        List<IMenuItem> mSubMenuItems = new List<IMenuItem>();
        string mText;
        bool mEnabled = true;
        bool mChecked = false;

        #endregion

        internal MenuItem(string text)
        {
            mText = text;
        }

        internal void AddMenuItem(IMenuItem menuItem)
        {
            mSubMenuItems.Add(menuItem);
        }

        internal bool Checked
        {
            get { return mChecked; }
            set { mChecked = value; }
        }

        internal bool Enabled
        {
            get { return mEnabled; }
            set { mEnabled = value; }
        }

        internal string Text
        {
            get { return mText; }
            set { mText = value; }
        }


        #region IMenuItem Members

        string IMenuItem.Text
        {
            get
            {
                return mText;
            }
        }

        bool IMenuItem.Checked
        {
            get
            {
                return mChecked;
            }
        }

        bool IMenuItem.Enabled
        {
            get
            {
                return mEnabled;
            }
        }

        IList<IMenuItem> IMenuItem.SubMenuItems
        {
            get
            {
                return mSubMenuItems;
            }
        }

        #endregion
    }
}

