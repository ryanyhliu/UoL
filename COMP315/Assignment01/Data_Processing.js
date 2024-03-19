const fs = require('fs'); // 引入文件系统模块
const path = require('path'); // 引入路径模块

class Data_Processing {

    constructor() { // 构造函数
        // 定义全局变量
        this.raw_user_data = []; // 用于存储原始用户数据的数组
        this.formatted_user_data = []; // 用于存储格式化后的用户数据的数组
        this.cleaned_user_data = []; // 用于存储清理后的用户数据的数组
        this.emailCounter = {}; // 用于存储电子邮件地址计数的对象
    }

    load_CSV(filename) {
        this.raw_user_data = fs.readFileSync(filename+".csv", 'utf8');
    }
    
    

    format_name(fullName) {
        let nameParts = fullName.trim().split(/\s+/); // 根据空白字符分割姓名
        let title = "", firstName = "", middleName = "", surname = ""; 

        // 检查首个词是否为常见的称谓，如果不是，则假设没有称谓
        const titles = ["Mr", "Mrs", "Miss", "Ms", "Dr"];
        if (titles.includes(nameParts[0])) { // 如果数组的第一个元素是称谓, 则移除并获取数组的第一个元素作为称谓
            title = nameParts.shift(); 
        }

        if (nameParts.length === 1) { // 如果数组只有一个元素, 则该元素可能是姓
            surname = nameParts[0];
        } else if (nameParts.length === 2) { // 如果数组有两个元素, 则第一个元素可能是名, 第二个元素可能是姓
            firstName = nameParts[0];
            surname = nameParts[1];
        } else if (nameParts.length > 2) { // 如果数组有三个或更多元素, 则第一个元素可能是名, 中间元素可能是中间名, 最后一个元素可能是姓
            firstName = nameParts[0];
            middleName = nameParts.slice(1, nameParts.length - 1).join(' '); // 将中间名的数组元素连接为字符串
            surname = nameParts[nameParts.length - 1];
        }

        return {
            title,
            first_name: this.capitalize(firstName),
            middle_name: this.capitalize(middleName, true),
            surname: this.capitalize(surname)
        };
    }
    
    capitalize(text, hyphenated = false) {
        if (!text) return "";
        if (hyphenated) {
            return text.split('-').map(part => part.charAt(0).toUpperCase() + part.slice(1).toLowerCase()).join('-');
        }
        return text.charAt(0).toUpperCase() + text.slice(1).toLowerCase();
    }

    format_age(age) {
        return parseInt(age, 10); // 直接转换年龄为整数
    }

    format_dateOfBirth(dateOfBirthStr) {
        let normalizedDate;
        if (dateOfBirthStr.includes('/')) {
            const parts = dateOfBirthStr.split('/');
            // 直接处理为 DD/MM/YYYY，考虑到输入可能已是此格式或为 DD/MM/YY
            if (parts.length === 3) {
                const year = parts[2].length === 2 ? `19${parts[2]}` : parts[2]; // 假设是19XX年代的年份
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
    

    
    calculateAge(dateOfBirthStr) {
        const convertedDateOfBirth = this.format_dateOfBirth(dateOfBirthStr);
    
        if (!convertedDateOfBirth) {
            throw new Error('Unsupported date format:' + dateOfBirthStr);
        }
    
        const collectionDate = new Date('2024-02-26'); // 数据收集日期：2024年2月26日
        const dateOfBirth = new Date(convertedDateOfBirth);
        const ageDif = collectionDate - dateOfBirth;
        const ageDate = new Date(ageDif);
        return Math.abs(ageDate.getUTCFullYear() - 1970);
    }
    

    processEmail(firstName, surname, providedEmail) {
        // 直接将名字和姓氏转换为小写，用于比较
        firstName = firstName.toLowerCase();
        surname = surname.toLowerCase();
    
        // 检查提供的电子邮件是否有效且符合预期格式 (检查后缀)
        const isSuffixValid = providedEmail && providedEmail.endsWith('@example.com');

        // 检查电子邮件的前缀是否符合 firstName.surname 的格式 (检查前缀)
        const expectedPrefix = `${firstName}.${surname}`;
        const actualPrefix = providedEmail.substring(0, providedEmail.indexOf('@'));
        const isPrefixValid = actualPrefix === expectedPrefix;

        // 检查电子邮件地址是否有效 (后缀 + 前缀)
        const isValidEmail = isSuffixValid && isPrefixValid;
    
        // 如果电子邮件地址有效，则使用提供的电子邮件地址，否则生成一个电子邮件地址
        let candidateEmail = isValidEmail ? providedEmail : `${expectedPrefix}@example.com`;
    
        // 检查候选电子邮件是否需要处理重复
        const count = this.emailCounter[candidateEmail] || 0;
        
        // 如果这个地址已经出现过，则在本地部分添加一个数字来区分
        if (count >= 1) {
            // 如果候选电子邮件已存在，生成一个带数字的新地址
            candidateEmail = `${expectedPrefix}${count}@example.com`;
        }

        this.emailCounter[candidateEmail] = count + 1;
        
        return candidateEmail;
    }

    extractNameFromEmail(email) {
        const nameParts = email.split('@')[0].split('.'); // 在@符号前分割电子邮件地址，然后在.号前分割名字部分
        let firstName = "", surname = "";
    
        if (nameParts.length >= 2) {
            firstName = nameParts[0];
            surname = nameParts.slice(1).join(' '); // 假设姓可能由多个部分组成
        } else {
            firstName = nameParts[0]; // 如果电子邮件中只有一个名字部分
        }
    
        // 将提取的名字部分标准化（首字母大写，其余小写）
        firstName = this.capitalize(firstName);
        surname = this.capitalize(surname);
    
        return { firstName, surname };
    }
    
    
    format_data() {
        const rows = this.raw_user_data.split('\n'); // 按行分割数据
        // 初始化一个数组来存储所有格式化后的用户数据
        this.formatted_user_data = rows.map(row => {
            if (row.trim()) {
                const values = row.split(','); // 按逗号分割每行数据
                // 使用 values 进行解构
                const [fullName, dob, age, email] = values;
    
                // 处理名字
                const nameParts = this.format_name(fullName);
    
                // 格式化出生日期
                const formattedDOB = this.format_dateOfBirth(dob);
    
                // 转换年龄为整数
                const ageInt = this.format_age(age);
    
                // 生成电子邮件地址
                const processedEmail = this.processEmail(nameParts.first_name, nameParts.surname, email);
    
                // 构建并返回格式化后的用户数据对象
                return {
                    title: nameParts.title,
                    first_name: nameParts.first_name,
                    middle_name: nameParts.middle_name,
                    surname: nameParts.surname,
                    date_of_birth: formattedDOB,
                    age: ageInt,
                    email: processedEmail
                };
            }
            return null;
        }).filter(user => user !== null); // 过滤掉任何未处理的行（即空行）
    }
    

    clean_data() {
        this.cleaned_user_data = this.formatted_user_data.map(user => {
            // 如果名字信息缺失，则尝试从电子邮件地址提取
            if (!user.first_name.trim() && !user.surname.trim() && user.email) {
                const { firstName, surname } = this.extractNameFromEmail(user.email);
                user.first_name = firstName;
                user.surname = surname;
            }

            // 重新计算年龄
            user.age = this.calculateAge(user.date_of_birth);

            // 返回清理后的用户数据对象
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
        let mostCommonSurname = "";
        let maxCount = 0;
        for (const surname in surnameCounter) {
            if (surnameCounter[surname] > maxCount) {
                mostCommonSurname = surname;
                maxCount = surnameCounter[surname];
            }
        }

        return mostCommonSurname;
    }

    average_age() {
        if (this.cleaned_user_data.length === 0) {
            return 0; // 如果没有数据，则平均年龄为0
        }
        
        // 累加所有用户的年龄
        const totalAge = this.cleaned_user_data.reduce((acc, user) => acc + user.age, 0);
        
        // 计算平均年龄，结果保留三位小数
        const avgAge = totalAge / this.cleaned_user_data.length;
        
        return Number(avgAge.toFixed(3)); // 将字符串结果转换回数字
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
            const month = user.date_of_birth.split('-')[1]; // 获取月份
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
    

    // 银行家舍入
    bankersRound(number) {
        var rounded = Math.round(number);
        if (Math.abs(number - rounded) === 0.5) {
            return (rounded % 2 === 0) ? rounded : rounded - 1;
        }
        return rounded;
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
