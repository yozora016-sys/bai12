# Bài
Chương trình minh họa cách sử dụng FreeRTOS Queue để truyền dữ liệu giữa các tác vụ (tasks).
Hệ thống có 2 task hoạt động song song:

Task 1 (LED Task):
Nhận dữ liệu từ Queue gồm tần số và độ rộng xung, sau đó nhấp nháy LED tương ứng.

Task 2 (Generator Task):
Định kỳ tạo ra giá trị tần số và độ rộng xung thay đổi, rồi gửi sang Queue cho Task 1.
