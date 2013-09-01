//#import <Foundation/Foundation.h>
//#import "MPInteger.h"
//
//int main(int argc, const char * argv[]) {
//    
//    @autoreleasepool {
//        if (argc != 4) {
//            return 1;
//        }
//        
//        MPInteger *a = [[MPInteger alloc] initWithString:[NSString stringWithUTF8String:argv[1]]];
//        MPInteger *b = [[MPInteger alloc] initWithString:[NSString stringWithUTF8String:argv[3]]];
//        MPInteger *result;
//        
//        switch (argv[2][0])
//        {
//            case '+':
//                result = [a add:b];
//                break;
//            case '-':
//                result = [a subtract:b];
//                break;
//            case '*':
//                result = [a multiply:b];
//                break;
//            case '/':
//                result = [a divideBy:b];
//                break;
//            case '%':
//                result = [a modulus:b];
//                break;
//                
//            default:
//                return EXIT_FAILURE;
//        }
//        
//        printf("%s", [[result description] cStringUsingEncoding:NSUTF8StringEncoding]);
//    }
//    
//    return 0;
//}
