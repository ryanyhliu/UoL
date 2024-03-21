const fs = require('fs'); // 引入文件系统模块
const path = require('path'); // 引入路径模块

class Data_Processing {

    constructor() { // 构造函数
        // 定义全局变量
        this.raw_user_data = []; // 用于存储原始用户数据的数组
        this.formatted_user_data = []; // 用于存储格式化后的用户数据的数组
        this.cleaned_user_data = []; // 用于存储清理后的用户数据的数组

        this.intermediateData = new Map(); // 用于存储中间数据的数组
        this.emailCounts = {}; // 用于存储电子邮件地址计数的对象
        this.emailMap = new Map(); // 用于存储电子邮件地址映射的Map
        
    }

    load_CSV(filename) {
        this.raw_user_data = fs.readFileSync(filename+".csv", 'utf8');
    }
    
    
    format_name(fullName) {
        let nameParts = fullName.trim().split(/\s+/); // 根据空白字符分割姓名
        let title = "", firstName = "", middleName = "", surname = ""; 
    
        // 检查首个词是否为常见的称谓
        const titles = ["Mr", "Mrs", "Miss", "Ms", "Dr", "Dr."];
        if (titles.includes(nameParts[0])) {
            title = nameParts.shift(); // 移除并获取称谓
        }
    
        // 如果最后一个元素是空字符串，移除它
        if (nameParts[nameParts.length - 1] === "") {
            nameParts.pop();
        }
    
        if (nameParts.length === 1) {
            // 如果数组只有一个元素，且之前有称谓，我们假设它是名字
            firstName = nameParts[0];
        } else if (nameParts.length === 2) {
            firstName = nameParts[0];
            surname = nameParts[1];
        } else if (nameParts.length > 2) {
            firstName = nameParts[0];
            middleName = nameParts.slice(1, nameParts.length - 1).join(' ');
            surname = nameParts[nameParts.length - 1];
        }
    
        return {
            title,
            first_name: firstName,
            middle_name: middleName,
            surname: surname
        };
    }
    

    format_age(age) {
        // 检查年龄是否已经是数字
        if (!isNaN(parseInt(age, 10))) {
            return parseInt(age, 10);
        }
    
        // 英文数字到数字的映射
        const numberWords = {
            "one": 1, "two": 2, "three": 3, "four": 4, "five": 5, "six": 6, "seven": 7, "eight": 8, "nine": 9, 
            "eleven": 11, "twelve": 12, "thirteen": 13, "fourteen": 14, "fifteen": 15,  "sixteen": 16, "seventeen": 17, "eighteen": 18, "nineteen": 19, 
            "ten": 10, "twenty": 20, "thirty": 30, "forty": 40, "fifty": 50, "sixty": 60, "seventy": 70, "eighty": 80, "ninety": 90
        };
    
        // 将年龄字符串分割为单词，并使用映射表转换为数字
        const words = age.toLowerCase().split(/-|\s/);
        let total = 0;
        words.forEach(word => {
            total += numberWords[word] || 0;
        });
    
        return total;
    }
    

    format_dateOfBirth(dateOfBirthStr) {
        let normalizedDate;
        if (dateOfBirthStr.includes('/')) {
            const parts = dateOfBirthStr.split('/');
            // 直接处理为 DD/MM/YYYY，考虑到输入可能已是此格式或为 DD/MM/YY
            if (parts.length === 3) {
                const year = parts[2].length === 2 ? (parseInt(parts[2]) >= 25 ? `19${parts[2]}` : `20${parts[2]}`) : parts[2]; // 假设是19XX年代的年份
                normalizedDate = `${parts[0].padStart(2, '0')}/${parts[1].padStart(2, '0')}/${year}`;
            }
        } else {
            // 处理其他格式，例如 "10 April 1963"
            const dateParts = dateOfBirthStr.split(' ');
            if (dateParts.length === 3) {
                normalizedDate = `${dateParts[0].padStart(2, '0')}/${this.monthToNumber(dateParts[1])}/${dateParts[2]}`;
            }
        }
    
        return normalizedDate;
    }
    
    monthToNumber(month) {
        const months = ["January", "February", "March", "April", "May", "June",
                        "July", "August", "September", "October", "November", "December"];
        const monthIndex = months.indexOf(month) + 1;
        return monthIndex < 10 ? `0${monthIndex}` : `${monthIndex}`;
    }
    

    
    clean_getAgeByDOB(dateOfBirthStr) {
        const parts = dateOfBirthStr.split("/");
        // 转换日期字符串为YYYY-MM-DD格式
        const convertedDateOfBirth = `${parts[2]}-${parts[1]}-${parts[0]}`;
    
        if (!convertedDateOfBirth) {
            throw new Error('Unsupported date format:' + dateOfBirthStr);
        }
    
        const collectionDate = new Date('2024-02-26'); // 数据收集日期：2024年2月26日
        const dateOfBirth = new Date(convertedDateOfBirth);
        const ageDifMs = collectionDate - dateOfBirth; // 得到毫秒数差
        const ageDate = new Date(ageDifMs); 
        return Math.abs(ageDate.getUTCFullYear() - 1970);
    }
    
    

    clean_getEmailByName(firstName, surname) {
        return `${firstName}.${surname}@example.com`;
    }
    

    clean_getNameByEmail(email) {
        // 在@符号前分割电子邮件地址，然后在.号前分割名字部分
        const nameParts = email.split('@')[0].split('.');
        let firstName = "", surname = "";
        
        // 由于电子邮件格式为firstname.surname，直接从nameParts分配
        if (nameParts.length === 2) {
            firstName = nameParts[0];
            surname = nameParts[1];
        }
    
        return { firstName, surname };
    }
    
    clean_removeTitleDots(user) {
        user.title = user.title.replace(".", "");
    }

    format_data() {
        const rows = this.raw_user_data.split('\n');
        rows.forEach(row => {
            if (row.trim()) {
                const values = row.split(',').map(item => item.trim());
                const [fullName, dob, age, email] = values;

                // 这里假设已经定义了format_name, format_dateOfBirth, 和 format_age方法
                const nameParts = this.format_name(fullName);
                const formatDOB = this.format_dateOfBirth(dob);
                const formatAge = this.format_age(age);
                const formatEmail = email.trim();

                const user = {
                    title: nameParts.title,
                    first_name: nameParts.first_name,
                    middle_name: nameParts.middle_name,
                    surname: nameParts.surname,
                    date_of_birth: formatDOB,
                    age: formatAge,
                    email: formatEmail
                };

                // 将处理后的数据添加到formatted_user_data
                this.formatted_user_data.push(user);

                let temp_date_of_birth = user.date_of_birth;
                let temp_first_name = user.first_name;
                let temp_surname = user.surname;
                let temp_email = user.email;

                // 信息补全: 从电子邮件提取姓名
                if ((!temp_first_name || !temp_surname) && temp_email) {
                    const temp_nameParts = this.clean_getNameByEmail(temp_email);
                    temp_first_name = temp_nameParts.firstName;
                    temp_surname = temp_nameParts.surname;
                }

                // 补全电子邮件
                if (!temp_email || temp_email.trim() === "@example.com") {
                    temp_email = this.clean_getEmailByName(temp_first_name, temp_surname);
                } else if (!temp_email.endsWith('@example.com')) {
                    temp_email = `${temp_email.split('@')[0]}@example.com`;
                }

                // 使用first_name, surname, 和 date_of_birth作为键进行去重
                const uniqueKey = `${temp_first_name}|${temp_surname}|${temp_date_of_birth}`;
                if (!this.intermediateData.has(uniqueKey)) {
                    this.intermediateData.set(uniqueKey, user);

                    // 在format_data方法中，统计每个电子邮件地址的出现次数
                    const emailKey = temp_email;
                    this.emailCounts[emailKey] = (this.emailCounts[emailKey] || 0) + 1;
                }
                
                
            }
        });
    }

    clean_data() {
        const emailSuffixCounter = {};

        this.cleaned_user_data = Array.from(this.intermediateData.values()).map(user => {
            
            // 信息补全: 从电子邮件提取姓名
            if ((!user.first_name || !user.surname) && user.email) {
                const nameParts = this.clean_getNameByEmail(user.email);
                user.first_name = nameParts.firstName;
                user.surname = nameParts.surname;
            }

            // 去除Title中的点
            this.clean_removeTitleDots(user);

            // 重新计算年龄
            user.age = this.clean_getAgeByDOB(user.date_of_birth);

            // 补全电子邮件
            if (!user.email || user.email.trim() === "@example.com") {
                user.email = this.clean_getEmailByName(user.first_name, user.surname);
            } else if (!user.email.endsWith('@example.com')) {
                user.email = `${user.email.split('@')[0]}@example.com`;
            }
            

            // 在clean_data方法中，为重复的电子邮件添加后缀
            const emailKey = user.email;
            if (this.emailCounts[emailKey] > 1) {
                emailSuffixCounter[emailKey] = (emailSuffixCounter[emailKey] || 0) + 1;
                user.email = `${user.email.split('@')[0]}${emailSuffixCounter[emailKey]}@${user.email.split('@')[1]}`;
            }

            return user;
        });
    }

    

    // 查询部分

    most_common_surname() {
        const surnameCounter = {}; // 用于存储姓氏计数的对象
        this.cleaned_user_data.forEach(user => {
            const surname = user.surname;
            if (surnameCounter[surname]) {
                surnameCounter[surname] += 1;
            } else {
                surnameCounter[surname] = 1;
            }
        });
    
        // 获取最常见的姓氏
        let mostCommonSurnames = [];
        let maxCount = 0;
        for (const surname in surnameCounter) {
            if (surnameCounter[surname] > maxCount) {
                mostCommonSurnames = [surname];
                maxCount = surnameCounter[surname];
            } else if (surnameCounter[surname] === maxCount) {
                mostCommonSurnames.push(surname);
            }
        }
    
        return mostCommonSurnames;
    }

    average_age() {
        if (this.cleaned_user_data.length === 0) {
            return 0; // 如果没有数据，则平均年龄为0
        }
        
        // 累加所有用户的年龄
        const totalAge = this.cleaned_user_data.reduce((acc, user) => acc + user.age, 0);
        
        // 计算平均年龄，结果保留一位小数
        const avgAge = totalAge / this.cleaned_user_data.length;
        
        return Number(avgAge.toFixed(1)); // 将字符串结果转换回数字
    }

    youngest_dr() {
        // 筛选出所有头衔为 "Dr." 的用户
        const doctors = this.cleaned_user_data.filter(user => user.title === "Dr");

        // 如果没有博士，则返回空对象
        if (doctors.length === 0) {
            return null;
        }

        // 获取年龄最小的博士
        let youngestDr = doctors[0];
        doctors.forEach(doctor => {
            if (doctor.age < youngestDr.age) {
                youngestDr = doctor;
            }
        });

        return youngestDr;
    }
    
    most_common_month() {
        const monthCounts = {};
        this.cleaned_user_data.forEach(user => {
            const month = user.date_of_birth.split('/')[1]; // 获取月份
            if (monthCounts[month]) {
                monthCounts[month]++;
            } else {
                monthCounts[month] = 1;
            }
        });
    
        let mostCommonMonth = '';
        let maxCount = 0;
        for (const month in monthCounts) {
            if (monthCounts[month] > maxCount) {
                mostCommonMonth = month;
                maxCount = monthCounts[month];
            }
        }
    
        return mostCommonMonth; // 直接返回最常见的月份数字
    }


    // 银行家舍入
    bankersRound(number) {
        var rounded = Math.round(number);
        if (Math.abs(number - rounded) === 0.5) {
            return (rounded % 2 === 0) ? rounded : rounded - 1;
        }
        return rounded;
    }
    
    percentage_titles() {
        const titleCounts = {};
        let totalUsers = this.cleaned_user_data.length;
    
        // 统计每个标题的出现次数
        this.cleaned_user_data.forEach(user => {
            const title = user.title || "blank"; // 对于缺失的标题使用"blank"
            titleCounts[title] = (titleCounts[title] || 0) + 1;
        });
    
        // 计算标题百分比
        const titlesPercentage = Object.keys(titleCounts).map(title => {
            const count = titleCounts[title];
            const percentage = (count / totalUsers) * 100;
            return {
                title,
                percentage: bankersRound(percentage) // 使用银行家舍入到最接近的整数
            };
        });
    
        return titlesPercentage;
    }
    
    
    percentage_altered() {
        let changes = 0;
        const total = this.formatted_user_data.length * Object.keys(this.formatted_user_data[0]).length;
    
        for (let i = 0; i < this.formatted_user_data.length; i++) { // 遍历每个用户的格式化数据
            Object.keys(this.formatted_user_data[i]).forEach(key => {
                if (this.formatted_user_data[i][key] !== this.cleaned_user_data[i][key]) {
                    changes++;
                }
            });
        }
    
        const percentage = (changes / total) * 100;
        return percentage.toFixed(2); // 返回修改的百分比，保留两位小数
    }
    
}


// const dataProcessor = new Data_Processing();

// dataProcessor.load_CSV('Raw_User_Data.csv');
// // 读取raw_user_data
// console.log(dataProcessor.raw_user_data);

// age = dataProcessor.calculateAge('11/01/1986');
// console.log(age);
