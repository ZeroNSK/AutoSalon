#ifndef PERMISSION_MANAGER_H
#define PERMISSION_MANAGER_H

#include "user.h"

class PermissionManager {
public:
     
    static bool canViewCars(UserRole role) {
        return true;  
    }
    
     
    static bool canAddCars(UserRole role) {
        return role >= UserRole::Manager;
    }
    
     
    static bool canEditCars(UserRole role) {
        return role >= UserRole::Manager;
    }
    
     
    static bool canDeleteCar(UserRole role) {
        return role >= UserRole::Manager;
    }
    
     
    static bool canMassDelete(UserRole role) {
        return role >= UserRole::Admin;
    }
    
     
    static bool canUpdatePrices(UserRole role) {
        return role >= UserRole::Manager;
    }
    
     
    static bool canExportData(UserRole role) {
        return role >= UserRole::Seller;
    }
    
     
    static bool canViewFiles(UserRole role) {
        return role >= UserRole::Seller;
    }
    
     
    static bool canFilterData(UserRole role) {
        return true;  
    }
    
     
    static bool canManageUsers(UserRole role) {
        return role >= UserRole::Admin;
    }
    
     
    static QString getRoleDescription(UserRole role) {
        switch (role) {
            case UserRole::Guest:
                return "Просмотр каталога автомобилей";
            case UserRole::Seller:
                return "Просмотр, фильтрация, экспорт данных";
            case UserRole::Manager:
                return "Управление инвентарем (добавление, редактирование, удаление)";
            case UserRole::Admin:
                return "Полный доступ к системе";
            default:
                return "Неопределенные права";
        }
    }
};

#endif  