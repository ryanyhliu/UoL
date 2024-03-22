const { log } = require('console');
const fs = require('fs'); // 引入文件系统模块
const path = require('path'); // 引入路径模块

class Data_Processing {

    constructor() { 
        this.raw_user_data = []; 
        this.formatted_user_data = []; 
        this.cleaned_user_data = []; 

        this.intermediateData = new Map(); // store intermediate data for duplicate removal
        this.emailCounts = {}; // count the number of times each email appears (first pass)
        this.emailMap = new Map(); // add suffixes to duplicate emails (second pass)
        
    }

    load_CSV(filename) {
        this.raw_user_data = fs.readFileSync(filename + ".csv", 'utf8');
    }
    
    
    format_name(fullName) {
        let nameParts = fullName.trim().split(" "); // spilt by space
        let title = "";
        let firstName = "";
        let middleName = "";
        let surname = ""; 
    
        // check if the first element is a title
        const titles = ["Mr", "Mrs", "Miss", "Ms", "Dr", "Dr."];
        if (titles.includes(nameParts[0])) {
            title = nameParts.shift(); // remove the title from the array
        }
    
        // remove any empty strings from the array
        if (nameParts[nameParts.length - 1] === "") {
            nameParts.pop();
        }
    
        // assign the remaining parts to the correct variables
        if (nameParts.length === 1) {
            firstName = nameParts[0];
        } else if (nameParts.length === 2) {
            firstName = nameParts[0];
            surname = nameParts[1];
        } else if (nameParts.length > 2) { // if more than 2 parts, assume the last part is the surname
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
        // check if the age is already a number
        if (!isNaN(parseInt(age, 10))) {
            return parseInt(age, 10);
        }
    
        // map of number words to their integer values
        const numberWords = {
            "one": 1, "two": 2, "three": 3, "four": 4, "five": 5, "six": 6, "seven": 7, "eight": 8, "nine": 9, 
            "eleven": 11, "twelve": 12, "thirteen": 13, "fourteen": 14, "fifteen": 15,  "sixteen": 16, "seventeen": 17, "eighteen": 18, "nineteen": 19, 
            "ten": 10, "twenty": 20, "thirty": 30, "forty": 40, "fifty": 50, "sixty": 60, "seventy": 70, "eighty": 80, "ninety": 90
        };
    
        // split the age string into words and sum the values
        const words = age.toLowerCase().split('-').join(' ').split(' '); // split by hyphen or space
        let total = 0;
        words.forEach(word => {
            total += numberWords[word] || 0; // add the number value or 0 if not found
        });
    
        return total;
    }
    

    format_dateOfBirth(str_DOB) {
        let formattedDate;
        if (str_DOB.includes('/')) {
            const parts = str_DOB.split('/');
            // date of format DD/MM/YYYY
            if (parts.length === 3) {
                // Split the date into parts (assuming 'parts' is already defined as the date split into [day, month, year])
                const isTwoDigitYear = parts[2].length === 2; // Check if the year part of the date has only two digits

                let year;
                if (isTwoDigitYear) {
                const twoDigitYear = parseInt(parts[2]); // Convert the two-digit year to an integer
                
                
                if (twoDigitYear >= 25) { // If the year is 25 or later, assume it's 19XX
                    year = `19${parts[2]}`;
                } else { // Otherwise, assume it's the 20XX
                    year = `20${parts[2]}`;
                }
                } else {
                year = parts[2]; // If the year is not two digits, use it as is
                }

                // Format the date components to ensure two digits for month and day
                const monthPadded = parts[0].padStart(2, '0'); // Pad the month to ensure it has two digits
                const dayPadded = parts[1].padStart(2, '0'); // Pad the day to ensure it has two digits

                formattedDate = `${monthPadded}/${dayPadded}/${year}`;

            }
        } else {
            // deal with date of format "DD Month YYYY"
            const dateParts = str_DOB.split(' ');
            if (dateParts.length === 3) {
                const dayPadded = dateParts[0].padStart(2, '0');
                const monthNumber = this.monthToNumber(dateParts[1]).toString().padStart(2, '0');
                const year = dateParts[2];

                formattedDate = `${dayPadded}/${monthNumber}/${year}`;
            }
        }
    
        return formattedDate;
    }
    
    monthToNumber(month) {
        const months = ["January", "February", "March", "April", "May", "June",
                        "July", "August", "September", "October", "November", "December"];
        const monthIndex = months.indexOf(month) + 1;
        return monthIndex < 10 ? `0${monthIndex}` : `${monthIndex}`;
    }
    

    
    clean_getAgeByDOB(dateOfBirthStr) {
        const parts = dateOfBirthStr.split("/");
        // get the date of birth in the format YYYY-MM-DD
        const convertedDateOfBirth = `${parts[2]}-${parts[1]}-${parts[0]}`;
    
        if (!convertedDateOfBirth) {
            throw new Error('Unsupported date format:' + dateOfBirthStr);
        }
    
        const collectionDate = new Date('2024-02-26'); 
        const dateOfBirth = new Date(convertedDateOfBirth);
        const ageDiff = collectionDate - dateOfBirth;
        const ageDate = new Date(ageDiff); 
        return Math.abs(ageDate.getUTCFullYear() - 1970);
    }
    
    

    clean_getEmailByName(firstName, surname) {
        return `${firstName}.${surname}@example.com`;
    }
    

    clean_getNameByEmail(email) {
        // split with '@' and '.' to get the name parts, i.e. firstname.surname
        const nameParts = email.split('@')[0].split('.');
        let firstName = "";
        let surname = "";
        
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
                const values = row.split(',').map(item => item.trim()); // split by comma and remove space
                const [fullName, dob, age, email] = values;

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


                // construct a unique key to remove duplicates and count email occurrences
                let temp_date_of_birth = JSON.parse(JSON.stringify(user.date_of_birth)); // deep copy !!!
                let temp_first_name = JSON.parse(JSON.stringify(user.first_name));
                let temp_surname = JSON.parse(JSON.stringify(user.surname));
                let temp_email = JSON.parse(JSON.stringify(user.email));

                // extract name from email
                if ((!temp_first_name || !temp_surname) && temp_email) { 
                    const temp_nameParts = this.clean_getNameByEmail(temp_email);
                    temp_first_name = temp_nameParts.firstName;
                    temp_surname = temp_nameParts.surname;
                }

                // complete email
                if (!temp_email || temp_email.trim() === "@example.com") {
                    temp_email = this.clean_getEmailByName(temp_first_name, temp_surname);
                } else if (!temp_email.endsWith('@example.com')) {
                    temp_email = `${temp_email.split('@')[0]}@example.com`;
                }

                // remove depulicates
                const uniqueKey = `${temp_first_name}|${temp_surname}|${temp_date_of_birth}`;
                if (!this.intermediateData.has(uniqueKey)) {
                    this.intermediateData.set(uniqueKey, user);

                    // count email occurrences
                    const emailKey = temp_email;
                    this.emailCounts[emailKey] = (this.emailCounts[emailKey] || 0) + 1;
                }
                
                
            }
        });
    }

    clean_data() {
        const temp_formatted_user_data = JSON.parse(JSON.stringify(this.formatted_user_data)); // deep copy

        const emailSuffixCounter = {};

        this.cleaned_user_data = Array.from(this.intermediateData.values()).map(user => {
            
            // process the name
            if ((!user.first_name || !user.surname) && user.email) {
                const nameParts = this.clean_getNameByEmail(user.email);
                user.first_name = nameParts.firstName;
                user.surname = nameParts.surname;
            }

            // process the title
            this.clean_removeTitleDots(user);

            // process the age
            user.age = this.clean_getAgeByDOB(user.date_of_birth);

            // process the email (missing or incorrect email)
            if (!user.email || user.email.trim() === "@example.com") {
                user.email = this.clean_getEmailByName(user.first_name, user.surname);
            } else if (!user.email.endsWith('@example.com')) {
                user.email = `${user.email.split('@')[0]}@example.com`;
            }
            

            // process duplicate emails
            const emailKey = user.email;
            if (this.emailCounts[emailKey] > 1) {
                emailSuffixCounter[emailKey] = (emailSuffixCounter[emailKey] || 0) + 1;
                user.email = `${user.email.split('@')[0]}${emailSuffixCounter[emailKey]}@${user.email.split('@')[1]}`;
            }

            return user;
        });

        this.formatted_user_data = temp_formatted_user_data;
    }

    

    //---------------------- Query Functions ----------------------

    most_common_surname() {
        const surnameCounter = {}; // store the count of each surname
        this.cleaned_user_data.forEach(user => {
            const surname = user.surname;
            if (surnameCounter[surname]) {
                surnameCounter[surname] += 1;
            } else {
                surnameCounter[surname] = 1;
            }
        });
    
        // get the most common surname
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
        let totalAge = 0;

        this.cleaned_user_data.forEach(user => {
            totalAge += user.age;
        });
        
        const avgAge = totalAge / this.cleaned_user_data.length;
        
        return Number(avgAge.toFixed(1)); 
    }

    youngest_dr() {
        // get all the doctors
        const doctors = this.cleaned_user_data.filter(user => user.title === "Dr");

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
            const month = user.date_of_birth.split('/')[1]; // get month
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
    
        return mostCommonMonth; 
    }


    // bankersRound: 银行家舍入法
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
    
        // count the number of each title
        this.cleaned_user_data.forEach(user => {
            const title = user.title || "blank"; // use 'blank' if title is missing
            titleCounts[title] = (titleCounts[title] || 0) + 1;
        });
    
        // order of titles
        const titleOrder = ['Mr', 'Mrs', 'Miss', 'Ms', 'Dr', 'blank'];
    
        // calculate the percentage of each title
        const titlesPercentage = titleOrder.map(title => {
            const count = titleCounts[title] || 0;
            const percentage = (count / totalUsers) * 100;
            return this.bankersRound(percentage); 
        });
    
        return titlesPercentage;
    }
    

    percentage_altered() {
        let changes = 0;
        const numOfKeys = 7;
        const total = this.formatted_user_data.length * numOfKeys;

        for (let i = 0; i < this.cleaned_user_data.length; i++) {
            let found = false;
            // match the user in cleaned data and formatted data with the same name or email and date of birth
            if (((this.cleaned_user_data[i].first_name === this.formatted_user_data[i].first_name &&
                this.cleaned_user_data[i].surname === this.formatted_user_data[i].surname) ||
                this.cleaned_user_data[i].email.split('@')[0].replace(/\d+$/, '') === this.formatted_user_data[i].email.split('@')[0].replace(/\d+$/, '')) && 
                this.cleaned_user_data[i].date_of_birth === this.formatted_user_data[i].date_of_birth) {
                found = true;
                Object.keys(this.cleaned_user_data[i]).forEach(key => {
                    if (this.cleaned_user_data[i][key] !== this.formatted_user_data[i][key]) {
                        changes++;
                    }
                });
            }
        }

        // calculate the removed duplicates
        changes += numOfKeys * (this.formatted_user_data.length - this.cleaned_user_data.length);

        
        const percentage = (changes / total) * 100;
        return Number(percentage).toPrecision(3);
    }



}


// const dataProcessor = new Data_Processing();

// // dataProcessor.load_CSV('Raw_User_Data.csv');
// // // 读取raw_user_data
// // console.log(dataProcessor.raw_user_data);

// // age = dataProcessor.calculateAge('11/01/1986');
// // console.log(age);

// dataProcessor.load_CSV('Raw_User_Data');
// dataProcessor.format_data();
// dataProcessor.clean_data();

// dataProcessor.percentage_altered();
// console.log(dataProcessor.percentage_altered());

// // export cleaned_user_data (filename = 'Cleaned_User_Data')
// // fs.writeFileSync('Cleaned_User_Data.json', JSON.stringify(dataProcessor.cleaned_user_data, null, 2));

// // export formatted_user_data (filename = 'Formatted_User_Data')

