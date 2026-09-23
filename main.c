int main() {
    // 初始化链表
    if (!initInitCardList()) {
        printf("链表初始化失败，程序无法继续！\n");
        return 1;
    }

    // 加载卡数据
    getCard();

    int choice;
    int nScanfResult;  // 添加：用于判断scanf返回值

    printf("欢迎进入计费管理系统\n");
    do {
        printf("\n----------菜单----------\n");
        outputMenu();

        // 修改：检查输入是否为数字
        nScanfResult = scanf_s("%d", &choice);

        // 如果输入不是数字，清空缓冲区并提示
        if (nScanfResult != 1) {
            printf("输入无效！请输入数字！\n");
            // 清空输入缓冲区
            while (getchar() != '\n');
            continue;
        }

        printf("\n\n");

        switch (choice) {
        case 1:
            add();
            break;
        case 2:
            query();
            break;
        case 3:
            logon();
            break;
        case 4:
            settle();
            break;
        case 5:
            addMoney();
            break;
        case 6:
            refundMoney();
            break;
        case 7:
            statistics();
            break;
        case 8:
            annul();
            break;
        case 0:
            exitAPP();
            printf("您选择了:退出\n感谢使用本计费管理系统,欢迎下次使用!");
            break;
        default:
            printf("您输入的菜单项有问题,请再次输入\n");
            break;
        }
        printf("\n\n\n");
    } while (choice != 0);

    return 0;
}